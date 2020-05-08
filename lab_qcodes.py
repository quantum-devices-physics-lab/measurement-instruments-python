import numpy as np
import logging
from qcodes.utils.validators import Numbers, Arrays
from qcodes.instrument.base import Instrument
from qcodes.instrument.parameter import ParameterWithSetpoints, Parameter
from qcodes.instrument.channel import InstrumentChannel


class DummySignalGenerator(Instrument):
    
    def __init__(self, name,ifreq=5,iamp=-5, **kwargs):

        super().__init__(name, **kwargs)
        
        self.add_parameter('freq',
                           initial_value=ifreq,
                           unit='Hz',
                           label='frequency',
                           vals=Numbers(0,2000),
                           get_cmd=None,
                           set_cmd=None)
        
        self.add_parameter('amp',
                           initial_value=iamp,
                           unit='mV',
                           label='amplitude',
                           vals=Numbers(-5000,5000),
                           get_cmd=None,
                           set_cmd=None)

class GeneratedSetPoints(Parameter):   
    def __init__(self, startparam, stopparam, numpointsparam, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._startparam = startparam
        self._stopparam = stopparam
        self._numpointsparam = numpointsparam

    def get_raw(self):
        return np.linspace(self._startparam(), self._stopparam(),
                              self._numpointsparam())

class DummyArray(ParameterWithSetpoints):

    def get_raw(self):    
        npoints = self.instrument.n_points.get_latest()
        return_value = np.random.rand(npoints)
        if hasattr(self.instrument, 'other_inst_connected'):
            PSG = self.instrument.other_inst_connected
            freq = PSG.freq()
            amp =  PSG.amp()
            t_list = self.instrument.t_axis.get_latest()
            return_value += amp*np.sin(t_list*freq) 
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
                           vals=Numbers(1,5e4),
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
                   parameter_class=DummyArray,
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
