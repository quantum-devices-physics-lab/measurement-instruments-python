import os
from time import sleep
from datetime import datetime

#para plotar o grafico em real time
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style
#style.use('fivethirtyeight')

import numpy as np
import qcodes as qc
from qcodes import (
    Measurement,
    experiments,
    initialise_or_create_database_at,
    load_or_create_experiment,
)
from qcodes.dataset.plotting import plot_dataset
from qcodes.logger.logger import start_all_logging
from qcodes.tests.instrument_mocks import DummyInstrument
from qcodes.instrument.parameter import Parameter

#globals().clear()
#api prototype
#this class will be supposed to execute the tasks above and serve
#as an api that you can import in a python instance or file and execute
class One_tone:
    def __init__(self, database="./OneTone.db"):
            #sets the defaults values
            self.database=database
            self.range=[0,60]
            self.samples=120
            self.amp=5
            self.Freq_time=0.01
            self.colors=['b','g','r','c','m','y','k']
            self.colorN=0
            self.ax1=None  #to share the graph screen through the class
           

            self.set_qcodes()
           #print defaults
            print("database", self.database)
            self.print_setup()

###### methods to change the experiment setup values #################

#the max and min frequency sweeped by the experiment
    def freq_range(self, interval):
            self.range=interval
            self.print_setup()

#amp of the input signal of the experiment
    def ampl(self, voltage):
            self.amp=voltage
            self.print_setup()
           
    #the time that the sweep stays at each frequency    
    def freq_time(self, interval):
            self.Freq_time=interval
            self.print_setup()


   ######################################################## 

    #sets up/resets the graph window to display as many graphs as wished on it
    def graph_window(self):
            #plot axis to make the points appear in the same graph
            plt.ion()
            fig = plt.figure()
            self.ax1 = fig.add_subplot()
            print("Window set up, run experiments to draw graphs")

    #loads an experiment to add the data to it instead of the one created automatically
    def load_exp(self):
            print ("exp loaded")

    def list_exps(self):
            exps=experiments()

    #loads a database to add the data to it instead of the one opened at the beggining
    def load_database(self):
            print ("database loaded")

    #starts experiment
    def run(self, graph=True, amp=-1):
            dac=self.dac
            dmm=self.dmm
            exp=self.exp
            station=self.station

        #selects between amp set previously or set in the run call
            if (amp==-1):
                dac.amp(self.amp)
            else:
                dac.amp(amp)

            meas= Measurement(exp=exp, station=station)
            meas.register_parameter(dac.freq)  # register the first independent parameter
            meas.register_parameter(dac.amp)
            meas.register_parameter(dmm.v1, setpoints=(dac.freq,))  # now register the dependent oone
            
            meas.write_period = 2 
            
            #vectors for plotting
            Y = [] 
            X = []

            with meas.run() as datasaver:
                #np.linspace is the interval and number of points in the osciloscope
                for set_v in np.linspace(self.range[0], self.range[1], self.samples):
                    #sets dac ch1 with the set_v value and gets value from v1
                    dac.freq.set(set_v)
                    get_v = dmm.v1.get()
                    #adds to the datasaver the result
                    datasaver.add_result((dac.freq, set_v),
                                         (dmm.v1, get_v))
            
                    #stays in one freq so that the osciloscope can measure the necessary
                    sleep(self.Freq_time)

                    #plots graph in real time

                    #checks if there is a canvas to plot in
                    if (self.ax1 != None ):
                        #gets data from dataset to plot
                        X.append(set_v)
                        Y.append(get_v) 
                       #plots data with color from array picked by number 
                        self.ax1.plot(X, Y,c=self.colors[self.colorN])
                        plt.pause(0.01)
            
                #changes color of plot for next time
                self.colorN=(self.colorN+1)%7
                #adds the amplitude to the dataset as well
                datasaver.add_result((dac.amp, dac.amp()))
            
                self.dataset = datasaver.dataset  # convenient to have for plotting
            
            print("experiment was run")

    #prints the experiment setup values        
    def print_setup(self):
        print("freq_range",self.range, "Hz | ampl", self.amp,
                  "V ")
        print("freq_time",self.Freq_time,"s")
    
    #sets up the qcodes code to run the experiment
    def set_qcodes(self):
         initialise_or_create_database_at(self.database)
         self.station = qc.Station()

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
         self.exp = load_or_create_experiment(experiment_name='One Tone',
                                sample_name=now)

         self.dmm.v1 = dmm_parameter('dmm_v1', self.dac)






###############################################################

#in this class is defined the simulated behavior of the osciloscope entrance
class dmm_parameter(qc.Parameter):
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
            
