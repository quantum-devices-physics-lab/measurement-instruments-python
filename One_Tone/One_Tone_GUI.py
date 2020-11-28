import PySimpleGUIQt as sg

import sys
import configparser
from multiprocessing import Process
import time

import One_Tone as ot

def graph_win():
    import time
    print("batata")
    exp = ot.One_tone()
    exp.graph_window()
    time.sleep(5)

#sg.theme('DarkAmber')
sg.theme('Dark Blue 3')

# conteudo da janela (aqui tambem se altera as chaves do dictionary)
layout = [
#experiment data
            [sg.Text('Frequency sweep range'),  sg.InputText(key="freq_range"), sg.Text('Hz')],
            [sg.Text('Frequency sweep samples'),  sg.InputText(key="freq_samples")],
            [sg.Text('Amplitude for frequency only sweep'),  sg.InputText(key="ampl"), sg.Text('V')],
            [sg.Text('Amplitude sweep range'),  sg.InputText(key="amp_range"), sg.Text('V')],
            [sg.Text('Amplitude sweep samples'),  sg.InputText(key="amp_samples")],
            [sg.Text('Duration of each frequency'),  sg.InputText(key="freq_time"), sg.Text('s')],

#area responsible for saving and loading files

            [sg.Text('Load file'), sg.InputText(key="load",default_text="./save.ini"), sg.FileBrowse(), sg.Button('Load')],
          [sg.Text('Save File'), sg.InputText(key="save",default_text="./save.ini"), sg.FileSaveAs(), sg.Button('Save')],
          [sg.Button('New Window') , sg.Button('Frequency Sweep'), sg.Button('Double Sweep')],
            [sg.Button('Ok') , sg.Button('Close')] ]

#initializes config object
config = configparser.ConfigParser()
#initializes experiment object
exp = ot.One_tone()
#initializes window
window = sg.Window('Dados do experimento', layout)

while True:
    event, values = window.read()
    if event in (None, 'Close'):# Ends execution
        window.close()
        sys.exit()
        
    elif (event == 'Load'):	# 
        config.read(values['load'])
        #sg.popup('Loaded path invalid')
        ler =config["One_Tone"]
        for keys in ler:#changes each window input with data from file
            window[keys](ler[keys])
        
    elif (event == 'Save'):	#
        location=values['save']
        del values["Browse"] #remove cells not important to save in the file
        del values["Save As..."]
        del values["save"]
        del values["load"]

        config['One_Tone']=values
        with open(location, 'w') as configfile:
            config.write(configfile)

        #sg.popup('Saved path invalid')
        
    elif (event == 'New Window'): 
        #loads a new graph canvas
        exp.graph_window()

    elif (event == 'Frequency Sweep'): 
        #passes data from window to experiment
        #converts string from window to list of ints
        exp.freq_range=list(map(float, values["freq_range"].strip('][').split(',') ))
        exp.freq_samples=int(values["freq_samples"])
        exp.freq_time=float(values["freq_time"])
        exp.ampl=float(values["ampl"])
        exp.run()


    elif (event == 'Double Sweep'): 
        #passes data from window to experiment
        exp.freq_range=list(map(float, values["freq_range"].strip('][').split(',') ))
        exp.freq_samples=int(values["freq_samples"])
        exp.freq_time=float(values["freq_time"])

        exp.amp_range=list(map(float, values["amp_range"].strip('][').split(',') ))
        exp.amp_samples=int(values["amp_samples"])
        #new window and runs
        exp.graph_window()
        exp.double_sweep()

    elif (event == 'Ok'): #
        del values["Browse"] #
        del values["Save As..."]
        del values["save"]
        del values["load"]
        saida=values
        break

window.close()
