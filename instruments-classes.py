def unpackData(data,waveformat):
    datatype_size = {'BYTE':1,'WORD':2}
    datatype = {'BYTE':'b','WORD':'h'}
    nchar = int(chr(data[1]))
    nbytes_returned = int(data[2:nchar+2])
    padding = nchar+2
    return struct.unpack('>'+ 'x'*padding + datatype[waveformat] * (nbytes_returned//datatype_size[waveformat]) + 'x' , data)

class Instrument:
    def __init__(self,address,alias):
        """ Inicia comunicacao com o intrumento. """
        rm = visa.ResourceManager()
        self.visa = rm.open_resource(address)
        self._alias = alias

    def write(self, command):
        return self.visa.write(command)

    def query(self, command):
        return self.visa.query(command)

    def readRaw(self):
        return self.visa.read_raw()

    def open(self):
        return self.visa.open()

    def close(self):
        return self.visa.close()

class Source(Instrument):
    def __init__(self, address, alias):
        super().__init__(address, alias)

    def setFreq(self, freq):
        self.write('FREQ {}'.format(freq))

    def setAmp(self, amp):
        self.write(':POWER {} dBm'.format(amp))


class Oscilloscope(Instrument):
    def __init__(self, address, alias):
        super().__init__(address, alias)

    def setChan(self, channel):
        """Selects waveform source channel
        Input an integer 1-4"""
        self.write(':WAV:SOUR CHAN{}'.format(channel))

    def setOffset(self, channel, offset):
        """Sets the vertical value at center od display
        Input channel idn and offset value"""
        self.write(":CHANNEL{}:OFFSET {}".format(channel,offset))

    def setInterval(self, start, size):
        # acho que nao entendi direito ainda o que faz...
        self.write(':WAV:DATA? {},{}'.format(1,''))

    def getFormat(self):
        """Gets the waveform data output format"""
        self.query(':WAV:FORMAT?')[:-1]

    def stop(self):
        """Stop acquiring data"""
        self.write(":STOP")

    def setTimeRange(self, rnge):
        """Sets full-scale horizontal time
        (10 times time-per-division)
        Input real number"""
        self.write(':TIMEBASE:RANGE {}'.format(rnge))

    def setAqcMode(self, mode):
        """Sets the acquisition mode
        Input str ETIM RTIM PDET
        HRES SEGM SEGP SEGH """
        self.write('AQC:MODE {}'.format(mode))

    def setSegmAmount(self, num_segments):
        """Sets number of segments to
        acquire in sampling mode = SEGM
        Input int"""
        self.write('ACQ:SEGM:COUN {}'.format(sum_segments))

    def captureWaveform(self, channel,start=1,npoints=' '):
        source = 'CHAN'+str(channel)
        waveformat = self.query(':WAV:FORMAT?')[:-1]
        self.write(':WAV:SOUR {}'.format(source))
        self.write(':WAV:DATA? {},{}'.format(start,npoints))
        data = self.read_raw()
        Y = list(unpack_data(data,waveformat))
        return Y

    def scaleData(self, data_array, channel):
        self.set_channel(channel)
        origin = float(self.query("WAVEFORM:YORIGIN?")[:-1])
        increment = float(self.query("WAVEFORM:YINCREMENT")[:-1])
        return data_array*increment + origin


class Attenuator(Instrument):
    def __init__(self, address, alias):
        super().__init__(address, alias)

    def attenuate(self, att):
        d = att[0]
        u = att[1]
        self.write('ATT:BANK1:X {}'.format(u))
        self.write('ATT:BANK1:Y {}'.format(d*10))
