import os
from time import sleep
from datetime import datetime

#para plotar o grafico em real time
import matplotlib.pyplot as plt

import numpy as np
from qcodes import (
    Measurement,
    experiments,
    initialise_or_create_database_at,
    load_or_create_experiment,
    Station
)
from qcodes.logger.logger import start_all_logging
from qcodes.tests.instrument_mocks import DummyInstrument
from qcodes.instrument.parameter import Parameter

#api prototype
#this class will be supposed to execute the tasks above and serve
#as an api that you can import in a python instance or file and execute
class One_tone:
    def __init__(self, database="./OneTone.db", exp_name="One_Tone"):
            #sets the defaults values
            self.__database=database
            self.__exp_name=exp_name

            self.__amp=5
            self.__range=[0,60]
            self.__samples=120
            self.__freq_time=0.001

            self.__amplitude_range=[6,12]
            self.__amplitude_samples=12

            self.__colors=['b','g','r','c','m','y','k']
            self.__colorN=0
            self.__ax1=None  #to share the graph screen through the class
           

            self.__set_qcodes()
           #print defaults
            print("database", self.__database)
            self.print_setup()

    def help():
        print("To print the variables for the setup use the method print_setup()\
        In this function output you can see the methods that change each setup variable and their current values.\
                    Call each of these methods in order to change their corresponding values. \n\
                    Methods:\n\
                        print_setup()   prints the current setup of the experiment\n\
        \n\
                        For variables:\n\
                              freq_range=interval  the range of the sweep frequency from beggining to end in the form interval =[beggining, end]\n\
                              amp_range=interval   the range of the sweep amplitude for the double sweep method from beggining to end in the form interval =[beggining, end]\n\
                                freq_samples=number  the number of samples in the  for the frequency with number=integer  \n\
                                amp_samples=number   the number of samples in the double sweep for the amplitude with number=integer \n\
                                ampl(voltage)         the amplitude for a single sweep in the run() method \n\
                                freq_time=interval   the time duration at which each frequency stays during the sweep step \n\
        \
                        graph_window()         opens a window to graph one or more runs. You can open more than one window and the runs will always graph on the last window\n\
        \
                        run(graph=True, amp=-1, legend=True)        runs the experiment saving the data to the qcodes database and if graph is True(default) and there is a window open graphs the output too. amp if positive will change the amplitude for the run and legend turns on off the legend in the graph useful for multiple plots in the same window.\n\
        \
                        double_sweep(graph=True)                   same as run but sweeps both frequency and amplitude and if graph is True and there is a window graphs a pcolor with the data collected. \n\
        ")
###### methods to change the experiment setup values #################

#the max and min frequency sweeped by the experiment
#getter of the function  exp.freq_range  returns output
    @property
    def freq_range(self):
            return self.__range

#setter of the function use exp.freq_range=value 
    @freq_range.setter
    def freq_range(self, interval):
            self.__range=interval
            self.print_setup()

#the max and min frequency sweeped by the experiment
    @property
    def amp_range(self):
            return self.__amplitude_range
        
    @amp_range.setter
    def amp_range(self, interval):
            self.__amplitude_range=interval
            self.print_setup()

#amp of the input signal of the experiment
    @property
    def amp_samples(self):
            return self.__amplitude_samples
        
    @amp_samples.setter
    def amp_samples(self, number):
            self.__amplitude_samples=number
            self.print_setup()

#amp of the input signal of the experiment
    @property
    def freq_samples(self):
            return self.__samples
        
    @freq_samples.setter
    def freq_samples(self, number):
            self.__samples=number
            self.print_setup()

#amp of the input signal of the experiment
    @property
    def ampl(self):
            return self.__amp
        
    @ampl.setter
    def ampl(self, voltage):
            self.__amp=voltage
            self.print_setup()
           
    #the time that the sweep stays at each frequency    
    @property
    def freq_time(self):
            return self.__freq_time
        
    @freq_time.setter
    def freq_time(self, interval):
            self.__freq_time=interval
            self.print_setup()


   ######################################################## 

    #sets up/resets the graph window to display as many graphs as wished on it
    def graph_window(self):
            #plot axis to make the points appear in the same graph
            plt.ion()
            self.__fig = plt.figure()
            self.__ax1 = self.__fig.add_subplot()
            print("Window set up, run experiments to draw graphs")

    def list_exps(self):
            exps=experiments()

    #loads a database to add the data to it instead of the one opened at the beggining
    def load_database(self):
            print ("database loaded")

    #starts experiment
    def run(self, graph=True, amp=-1, legend=True):
            dac=self.dac
            dmm=self.dmm
            exp=self.exp
            station=self.station

        #selects between amp set previously or set in the run call
            if (amp==-1):
                dac.amp(self.__amp)
                amp=self.__amp
            else:
                dac.amp(amp)

            meas= Measurement(exp=exp, station=station)
            meas.register_parameter(dac.freq)  # register the first independent parameter
            meas.register_parameter(dac.amp)
            meas.register_parameter(dmm.v1, setpoints=(dac.freq,))  # now register the dependent oone
            
            meas.write_period = 2 

            #plot lables 
            if (self.__ax1 != None and graph  ):
                self.__ax1.set_xlabel("Frequency")
                self.__ax1.set_ylabel("Amplitude measured")
                title='One Tone Spectroscopy sweeping the frequency'
                self.__ax1.set_title(title)
                #sets legend with amplitude for each plot to appear
                first=legend
            #vectors for plotting
            Y = [] 
            X = []

            with meas.run() as datasaver:
                #np.linspace is the interval and number of points in the osciloscope
                for set_v in np.linspace(self.__range[0], self.__range[1], self.__samples):
                    #sets dac ch1 with the set_v value and gets value from v1
                    dac.freq.set(set_v)
                    get_v = dmm.v1.get()
                    #adds to the datasaver the result
                    datasaver.add_result((dac.freq, set_v),
                                         (dmm.v1, get_v))
            
                    #stays in one freq so that the osciloscope can measure the necessary
                    sleep(self.__freq_time/10)

                    #plots graph in real time

                    #checks if there is a canvas to plot in
                    if (self.__ax1 != None and graph):
                        #gets data from dataset to plot
                        X.append(set_v)
                        Y.append(get_v) 
                       #plots data with color from array picked by number 
                        self.__ax1.plot(X, Y,c=self.__colors[self.__colorN])
                        #plots once with a label and the legend for it
                        if(first):
                            self.__ax1.plot(X, Y,c=self.__colors[self.__colorN], label='Amplitude '+str(self.__amp)+'V')
                            plt.legend(loc='lower right')
                            first=False

                        plt.pause(0.01)
            
                #changes color of plot for next time
                self.__colorN=(self.__colorN+1)%7
                #adds the amplitude to the dataset as well
                datasaver.add_result((dac.amp, dac.amp()))
            
                self.dataset = datasaver.dataset  # convenient to have for plotting
            
            print("experiment was run")

    #starts experiment
    def double_sweep(self, graph=True):
            dac=self.dac
            dmm=self.dmm
            exp=self.exp
            station=self.station

            meas= Measurement(exp=exp, station=station)
            meas.register_parameter(dac.freq)  # register the first independent parameter
            meas.register_parameter(dac.amp)
            meas.register_parameter(dmm.v1, setpoints=(dac.freq,dac.amp, ))  # now register the dependent oone
            
            meas.write_period = 2 

            if (self.__ax1 != None and graph  ):
                self.__ax1.set_xlabel("Frequency of input")
                self.__ax1.set_ylabel("Amplitude of input")
                self.__ax1.set_title('One Tone Spectroscopy with double sweep')
                colorbar=False

            #vectors for plotting
            Y = [] 
            X = []
            C = []
            column=-1
            with meas.run() as datasaver:
                for set_v in np.linspace(self.__amplitude_range[0], self.__amplitude_range[1], self.__amplitude_samples):
                    if (self.__ax1 != None and graph  ):
                         C.append([])
                         column+=1
                         Y.append(set_v) 
                    dac.amp(set_v)
                    for set_f in np.linspace(self.__range[0], self.__range[1], self.__samples):
                        #sets dac ch1 with the set_v value and gets value from v1
                        dac.freq.set(set_f)
                        get_v = dmm.v1.get()
                        #adds to the datasaver the result
                        datasaver.add_result((dac.freq, set_f),
                                            (dac.amp, set_v),
                                             (dmm.v1, get_v))
            
                        #stays in one freq so that the osciloscope can measure the necessary
                        sleep(self.__freq_time)

                        #plots graph in real time

                        #checks if there is a canvas to plot in
                        if (self.__ax1 != None and graph  ):
                            #gets data from dataset to plot
                            X.append(set_f)
                            C[column].append(get_v)

                           #plots data with color from array picked by number 
                    #the pcolor doesn't do well with imcomplete lines/columns alongside imcomplete ones so it' at the end of the main loop 
                    if (self.__ax1 != None and graph  ):
                        #removes repeated colorbars
                        if(colorbar):
                            colorbar.remove()
                        graph_colors=self.__ax1.pcolor(X, Y, C)
                        colorbar= self.__fig.colorbar(graph_colors, ax=self.__ax1)
                        plt.pause(0.01)
                        X=[]
                #changes color of plot for next time
                self.__colorN=(self.__colorN+1)%7
                #adds the amplitude to the dataset as well
                datasaver.add_result((dac.amp, dac.amp()))
            
                self.dataset = datasaver.dataset  # convenient to have for plotting
            
            print("experiment was run")


    #prints the experiment setup values        
    def print_setup(self):
        print("freq_range",self.__range, "Hz | freq_samples", self.__samples, "|freq_time", self.__freq_time,"|ampl", self.__amp,"V")
        print("amp_range",self.__amplitude_range, "V | amp_samples", self.__amplitude_samples)
    
    #sets up the qcodes code to run the experiment
    def __set_qcodes(self):
         initialise_or_create_database_at(self.__database)
         self.station = Station()

         #### instruments needs change
         # A dummy instrument dac with two parameters ch1 and ch2
         self.dac = DummyInstrument('dac', gates=['amp'])
         
         # A dummy instrument dmm with two parameters ch1 and Ch2
         self.dmm = DummyInstrument('dmm', gates=['v1'])

         #These are the parameters which come ready to use from the intruments drivers
         #dac.add_parameter('amp',label='Amplitude', unit="V", get_cmd=None, set_cmd=None)
         self.dac.add_parameter('freq',label='Frequency', unit="Hz", get_cmd=None, set_cmd=None)

         #puts current time in a string to facilitate control of the samples
         #makes the sample name
         now = datetime.now()
         now = now.strftime("%Y-%m-%d_%H-%M-%S")
         print(now)
         #the experiment is a unit of data inside the database it's made 
         #out 
         self.exp = load_or_create_experiment(experiment_name=self.__exp_name,
                                sample_name=now)

         self.dmm.v1 = dmm_parameter('dmm_v1', self.dac)






###############################################################

#in this class is defined the simulated behavior of the osciloscope entrance
class dmm_parameter(Parameter):
    def __init__(self, name, dac):
        super().__init__(name)
        #assingns the generator to self.ed
        self.ed = self.exponential_decay(0.01, 0.01)
        next(self.ed)
        print(dac)
        self.dac=dac

    #gets value automatically from dac channel 1
    def get_raw(self):
        shift=0
        """
        This method is automatically wrapped to
        provide a ``get`` method on the parameter instance.
        """
        #if the amp and thus the power is greater than a value 
        #the qubit punches out the frequency
        if(self.dac.amp()>10):
            shift=5
        val = self.ed.send(self.dac.freq()-shift)*self.dac.amp()
        next(self.ed)
        return val

    #this function returns an generator which is a kind of iterable
    def exponential_decay(self, a: float, b: float):
        """
        Yields a*exp(-b*x) where x is put in
        """
        x = 0
        while True:
            x = yield
            #yield returns a generator
            yield a*np.exp(-b*(x-15)**2) + 0.02*a*np.random.randn()
            
