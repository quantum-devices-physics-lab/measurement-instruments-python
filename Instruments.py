import visa
import struct
import numpy as np
import time
import unittest
import logging
import pandas as pd
from matplotlib.pyplot import *

rm = visa.ResourceManager()

class Instrument:
    def __init__(self,resource_address,alias):
        self._inst = rm.open_resource(resource_address)
        self._idn = self._inst.query("*IDN?")[:-1]
        self._alias = alias
        
    def write(self,command):
        return self._inst.write(command)
        
    def query(self,command):
        return self._inst.query(command)
    
    def read_raw(self):
        return self._inst.read_raw()
    
    def open(self):
        return self._inst.open()
    
    def close(self):
        return self._inst.close()

class PSGAnalogSignalGenerator(Instrument):
    def __init__(self,resource_address,alias):
        super().__init__(resource_address,alias)
        self.stop()
        self.stop_mod()
        
    def start(self):
        self.write(":OUTPUT ON")
    
    def stop(self):
        self.write(":OUTPUT OFF")
        
    def start_mod(self):
        self.write(":OUTPUT:MOD ON")
        
    def stop_mod(self):
        self.write(":OUTPUT:MOD OFF")
        
    @property
    def is_mod_on(self):
        mod_on = self.query(":OUTPUT:MOD?").strip()
        return int(mod_on) == 1
    
    @property
    def is_on(self):
        on = self.query(":OUTPUT?").strip()
        return int(on) == 1
        
    @property
    def frequency(self):
        freq = self.query(':FREQ?').strip()
        return float(freq)
    
    @frequency.setter
    def frequency(self,freq):
        self.write(':FREQ {}'.format(freq))
        
    @property
    def amplitude(self):
        amp = self.query(':POWER?').strip()
        return float(amp)
    
    @amplitude.setter
    def amplitude(self,amp):
        self.write(':POWER {} dBm'.format(amp))
        
class DigitalStorageOscilloscope(Instrument):
    def __init__(self,resource_address,alias):
        super().__init__(resource_address,alias)
        self._channel_source = int(self.query(':WAV:SOUR?').strip()[-1])
        self._waveformat = self.query(':WAV:FORMAT?').strip()
        self._offset = float(self.query(":CHANNEL{}:OFFSET?".format(self._channel_source)).split()[0])
        self._timerange = float(self.query(":TIMEBASE:RANGE?").split()[0])
        self._yorigin = float(self.query("WAVEFORM:YORIGIN?").split()[0])
        self._yincrement = float(self.query("WAVEFORM:YINCREMENT?").split()[0])
        self._xorigin = float(self.query("WAVEFORM:XORIGIN?").split()[0])
        self._xincrement = float(self.query("WAVEFORM:XINCREMENT?").split()[0])
        self._sample_rate = float(self.query("ACQuire:SRATe:ANALog?".format(250e6)).split()[0])
    
    def unpack_data(self,data,waveformat):
        datatype_size = {'BYTE':1,'WORD':2}
        datatype = {'BYTE':'b','WORD':'h'}

        nchar = int(chr(data[1]))
        nbytes_returned = int(data[2:nchar+2])
        padding = nchar+2

        return struct.unpack('>'+ 'x'*padding + datatype[waveformat] * (nbytes_returned//datatype_size[waveformat]) + 'x' , data)

    def capture(self,start=1,npoints=' '):
        self.write(':WAV:DATA? {},{}'.format(start,npoints))
    
        data = self.read_raw()
        Y = list(self.unpack_data(data,self._waveformat))
        return Y
    
    def start(self):
        self.write(':RUN')
        
    def stop(self):
        self.write(':STOP')
    
    @property
    def waveformat(self):
        self._waveformat = self.query(':WAV:FORMAT?').strip()
        return self._waveformat
    
    @waveformat.setter
    def waveformat(self,form):
        self.write(':WAV:FORMAT {}'.format(form))
        self._waveformat = self.query(':WAV:FORMAT?').strip()
    
    @property
    def source(self):
        self._channel_source = int(self.query(':WAV:SOUR?').strip()[-1])
        return self._channel_source
    
    @source.setter
    def source(self,sour):
        self.write(':WAV:SOUR CHAN{}'.format(sour))
        self._channel_source = int(self.query(':WAV:SOUR?').strip()[-1])
        
    @property
    def offset(self):
        self._offset = self.query(":CHANNEL{}:OFFSET?".format(self._channel_source)).split()[0]
        return float(self._offset)
    
    @offset.setter
    def offset(self,offset):
        self.write(":CHANNEL{}:OFFSET {}".format(self._channel_source,offset))
        self._offset = float(self.query(":CHANNEL{}:OFFSET?".format(self._channel_source)).split()[0])
        
    @property
    def timeRange(self):
        self._timerange = float(self.query(":TIMEBASE:RANGE?").split()[0])
        return self._timerange
    
    @timeRange.setter
    def timeRange(self,timerange):
        self.write(":TIMEBASE:RANGE {}".format(timerange))
        self._timerange = float(self.query(":TIMEBASE:RANGE?").split()[0])
        
    @property
    def yOrigin(self):
        self._yorigin = float(self.query("WAVEFORM:YORIGIN?").split()[0])
        return self._yorigin
    
    @property
    def xIncrement(self):
        self._yincrement = float(self.query("WAVEFORM:YINCREMENT?").split()[0])
        return self._yincrement
    
    @property
    def xOrigin(self):
        self._xorigin = float(self.query("WAVEFORM:XORIGIN?").split()[0])
        return self._xorigin
    
    @property
    def xIncrement(self):
        self._xincrement = float(self.query("WAVEFORM:XINCREMENT?").split()[0])
        return self._xincrement
        
    @property
    def sampleRate(self):
        self._sample_rate = float(self.query("ACQuire:SRATe:ANALog?".format(250e6)).split()[0])
        return self._sample_rate
    
    @sampleRate.setter
    def sampleRate(self, samplerate):
        self.write("ACQuire:SRATe:ANALog {}".format(samplerate))
        self._sample_rate = float(self.query("ACQuire:SRATe:ANALog?".format(250e6)).split()[0])
    
