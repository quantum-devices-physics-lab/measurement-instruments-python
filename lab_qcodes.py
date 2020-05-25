import numpy as np
import logging
from qcodes.utils.validators import Numbers, Arrays
from qcodes.instrument.base import Instrument
from qcodes.instrument.parameter import ParameterWithSetpoints, Parameter
from qcodes.instrument.channel import InstrumentChannel
import scipy.signal as sp

class Circuit(Instrument):
    def __init__(self,name, **kwargs):
        super().__init__(name, **kwargs)

    def signal(self,tlist):
        pass

class DUT(Circuit):
    def function_amp(self,freq):
        #f = lambda x: (self._sigma/2)/((x-self._omega0)**2+(self._sigma/2)**2)/np.pi
        f = lambda x: np.exp(-(x-self.omega0())**2/(2*self.sigma()**2))
        y = f(freq)
        y = 1-0.9*y
        return y
    
    def function_phase(self,freq):
        return 0
        
    def __init__(self,name, input_signal,iomega0=100,isigma=1, **kwargs):
        super().__init__(name, **kwargs)
        
        self.add_parameter('omega0',
                           initial_value=iomega0,
                           unit='Hz',
                           label='frequency',
                           vals=Numbers(0,20e9),
                           get_cmd=None,
                           set_cmd=None)
        
        self.add_parameter('sigma',
                           initial_value=isigma,
                           unit='Hz',
                           label='FWHM',
                           vals=Numbers(0,20e9),
                           get_cmd=None,
                           set_cmd=None)
        
        self._input = input_signal
        
    def signal(self,tlist):
        freq = self._input.freq()
        amp = self._input.amp()
        phase = self._input.phase()
        npoints = len(tlist)
        noise = self._input.noise_function(npoints)
        
        Y = amp*self.function_amp(freq)*np.sin(tlist*freq+self.function_phase(freq)*phase) + noise
        
        return Y



class HomodyneCircuit(Circuit):
    def __init__(self,name,RF,LO,**kwargs):
        super().__init__(name, **kwargs)
        self._RF = RF
        self._LO = LO
        self.add_parameter('fc',
                           initial_value=10,
                           unit='Hz',
                           label='frequency',
                           vals=Numbers(0,2000),
                           get_cmd=None,
                           set_cmd=None)
        self.add_parameter('fs',
                   initial_value=250,
                   unit='Hz',
                   label='frequency',
                   vals=Numbers(0,1e4),
                   get_cmd=None,
                   set_cmd=None)
        self.add_parameter('order',
                   initial_value=10,
                   vals=Numbers(1,20),
                   get_cmd=None,
                   set_cmd=None)
            
    def signal(self,tlist):
        Y1 = self._RF.signal(tlist)
        Y2 = self._LO.signal(tlist)
        sos = sp.butter(self.order(), self.fc(), 'lp', fs=self.fs(), output='sos')
        filtered = sp.sosfilt(sos, Y1*Y2)
        return filtered


class DummySignalGenerator(Instrument):
    
    def __init__(self, name,ifreq=5,iamp=5,noise_function = np.random.rand, **kwargs):

        super().__init__(name, **kwargs)

        self.noise_function = noise_function
        
        self.add_parameter('freq',
                           initial_value=ifreq,
                           unit='Hz',
                           label='frequency',
                           vals=Numbers(0,20e9),
                           get_cmd=None,
                           set_cmd=None)

        self.add_parameter('phase',
                           initial_value = 0,
                           unit = 'rad',
                           vals=Numbers(0,np.pi),
                           get_cmd=None,
                           set_cmd=None)
        
        self.add_parameter('amp',
                           initial_value=iamp,
                           unit='mV',
                           label='amplitude',
                           vals=Numbers(-5000,5000),
                           get_cmd=None,
                           set_cmd=None)

    def signal(self,tlist):
        npoints = len(tlist)
        noise = self.noise_function(npoints)
        return self.amp()*np.sin(tlist*self.freq()+self.phase()) + noise
        
class GeneratedSetPoints(Parameter):   
    def __init__(self, startparam, stopparam, numpointsparam, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._startparam = startparam
        self._stopparam = stopparam
        self._numpointsparam = numpointsparam

    def get_raw(self):
        return np.linspace(self._startparam(), self._stopparam(),
                              self._numpointsparam())

class DummyArraySignal(ParameterWithSetpoints):

    def get_raw(self):    
        npoints = self.instrument.n_points.get_latest()
        return_value = np.random.rand(npoints)
        if hasattr(self.instrument, 'other_inst_connected'):
            PSG = self.instrument.other_inst_connected
            t_list = self.instrument.t_axis()
            return_value = PSG.signal(t_list)
        return return_value

class DummyOscilloscopeChannel(InstrumentChannel):

    def connect_inst(self,inst):
        self.other_inst_connected = inst
        

    def __init__(self, parent: Instrument,  name, channel, it_start=-1,ft_stop=1, in_points=501):

        super().__init__(parent, name)
        
       
        self.add_parameter('t_start',
                           initial_value=it_start,
                           unit='s',
                           label='t start',
                           vals=Numbers(-1e3,1e3),
                           get_cmd=None,
                           set_cmd=None)

        self.add_parameter('t_stop',
                           initial_value=ft_stop,
                           unit='s',
                           label='t stop',
                           vals=Numbers(-1e3,1e3),
                           get_cmd=None,
                           set_cmd=None)

        self.add_parameter('n_points',
                           unit='',
                           initial_value=in_points,
                           vals=Numbers(1,50e6),
                           get_cmd=None,
                           set_cmd=None)

        
        self.add_parameter('t_axis',
                           unit='s',
                           label='t Axis',
                           parameter_class=GeneratedSetPoints,
                           startparam=self.t_start,
                           stopparam=self.t_stop,
                           numpointsparam=self.n_points,
                           vals=Arrays(shape=(self.n_points.get_latest,)))
        
        self.add_parameter('wavesample',
                   unit='V',
                   setpoints=(self.t_axis,),
                   label='Wavesample',
                   parameter_class=DummyArraySignal,
                   vals=Arrays(shape=(self.n_points.get_latest,)))

        self.channel = channel
    

class DummyOscilloscope(Instrument):

    def __init__(self, name, **kwargs):

        super().__init__(name, **kwargs)

        channel1 = DummyOscilloscopeChannel(self, 'ch1','ch1');
        channel2 = DummyOscilloscopeChannel(self, 'ch2','ch2')
        channel3 = DummyOscilloscopeChannel(self, 'ch3','ch3');
        channel4 = DummyOscilloscopeChannel(self, 'ch4','ch4');

        self.add_submodule('ch1',channel1);
        self.add_submodule('ch2',channel2);
        self.add_submodule('ch3',channel3);
        self.add_submodule('ch4',channel4);
