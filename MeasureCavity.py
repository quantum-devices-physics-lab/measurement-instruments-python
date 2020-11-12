import visa
import numpy as np
import time
from Instrument import *
import os
import pandas
from os import listdir
from os.path import isfile, join
import re
import logging


logging.basicConfig(level = logging.DEBUG, filename='MeasureCavity.log', filemode='w', format='%(asctime)s - %(levelname)s - %(message)s')






logging.info("Initializing instruments")
att_addr = 'TCPIP0::169.254.101.101::inst0::INSTR'
na_addr = 'TCPIP0::169.254.101.105::inst0::INSTR'
att = Attenuator(att_addr,"attenuator")
na = NetworkAnalyzer(na_addr,"network_analyzer")

def log(message):
    print(message)
    logging.info(message)

def execute(file):
    f = open(file, "r")
    script = f.read()
    f.close()

    exec(script)
    
    na.smooth = 0
    na.averaging = 1
    na.data_array_format = "PLOG"
    
def count_time():
    mypath="."
    onlyfiles = [f for f in listdir(mypath) if isfile(join(mypath, f))]
    
    regexPattern = re.compile("\w*.lab")
    log("\nFiles to be executed:")
    total_expected_time = 0
    for file in onlyfiles:
        if bool(regexPattern.fullmatch(file)):
            log("  {}".format(file))
            execute(file)
            total_expected_time += na.average_points*na.sweep_time+1

    log("Total expected time: {}h:{}m:{}s".format(int(total_expected_time/3600),
                                                  int((total_expected_time%3600)/60),
                                                  int(total_expected_time%3600%60)))
    
    now = time.time()
    ty_res = time.localtime(total_expected_time+now)
    res = time.strftime("%d/%m/%y %Hh:%Mm:%Ss",ty_res)
    log("Expected to finish at {}".format(res))
    
def prepare_files():
    mypath="."
    onlyfiles = [f for f in listdir(mypath) if isfile(join(mypath, f))]
    regexPattern = re.compile("\w*.lab")
    
    for filename in onlyfiles:
        if bool(regexPattern.fullmatch(filename)):
            base  = filename.split(".")[0]
            filename_data = base + "_data.dat"
            filename_freq = base + "_freqs.dat"

            ffreq = open(filename_freq,"r")
            fdata = open(filename_data,"r")

            freq = ffreq.read()
            data = fdata.read()

            ffreq.close()
            fdata.close()
            os.remove(filename_freq)
            os.remove(filename_data)

            freq,mag,phase = parse_data(data,freq)


            # intialise data of lists. 
            data = {'Frequency':freq, 'LogMag': mag, 'Phase': phase} 

            # Create DataFrame 
            df = pandas.DataFrame(data) 

            df.to_csv(base+'.csv') 


def main():
    
    
    try:
        
        mypath="."
        onlyfiles = [f for f in listdir(mypath) if isfile(join(mypath, f))]

        log("\nInstrument: {}\n  address {};\n  IDN: {}".format(att._alias,att_addr,att._idn))
        log("Instrument: {}\n  address {};\n  IDN: {}".format(na._alias,na_addr,na._idn))

        count_time()
        
        regexPattern = re.compile("\w*.lab")

        log("\nRunning")
        for file in onlyfiles:
            if bool(regexPattern.fullmatch(file)):
                log("  Executing {}".format(file))
                execute(file)

                expected_time = na.average_points*na.sweep_time+1

                ty_res = time.gmtime(expected_time)
                res = time.strftime("%Hh:%Mm:%Ss",ty_res)
                log("    Expected time: {}".format(res))

                na.clear_averaging()
                time.sleep(expected_time)

                freqs = na.frequency_array()
                data = na.data_array()

                log("    Acquired data")
                log("    Saving data")
                filename_freqs = file.split('.')[0] + "_freqs.dat"
                filename_data = file.split('.')[0] + "_data.dat"
                log("    Saving to {} and {}".format(filename_freqs,filename_data))
                f_freq = open(filename_freqs,"w")
                f_freq.write(freqs)
                f_freq.close()
                f_freq = open(filename_data,"w")
                f_freq.write(data)
                f_freq.close()


                del freqs
                del data


        na.close()
        att.close()
        rm.close()
        
        log("Preparing csv files with dataframes")
        prepare_files()
    
    except Exception as e:
        logging.error("Exception occurred", exc_info=True)

if __name__ == "__main__":
    main()

