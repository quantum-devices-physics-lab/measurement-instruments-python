import numpy as np
import os
import time
from qcodes import Station
from qcodes.instrument.specialized_parameters import ElapsedTimeParameter
from qcodes.dataset.plotting import plot_dataset
from qcodes.utils.validators import Numbers, Arrays
from qcodes.instrument.base import Instrument
from qcodes.logger.logger import start_all_logging
from qcodes.dataset.measurements import Measurement
from qcodes.instrument.parameter import ParameterWithSetpoints, Parameter
from qcodes.dataset.sqlite.database import initialise_or_create_database_at
from qcodes.dataset.experiment_container import load_or_create_experiment
import matplotlib.pyplot as plt
import scipy.signal as sp
from lab_qcodes import *

start_all_logging()

PSG1 = DummySignalGenerator("SignalGenerator1",noise_function=np.zeros)
PSG2 = DummySignalGenerator("SignalGenerator2",noise_function=np.zeros)
PSG3 = DummySignalGenerator("SignalGenerator3",noise_function=np.zeros)
osc = DummyOscilloscope('oscilloscope')

aDUT = DUT("DUT",PSG1)
circuit = HomodyneCircuit("circuit",aDUT,PSG2)


osc.ch1.n_points(2500)
osc.ch2.n_points(2500)
osc.ch3.n_points(2500)
osc.ch4.n_points(2500)

osc.ch1.connect_inst(aDUT)
osc.ch2.connect_inst(PSG2)
osc.ch3.connect_inst(circuit)

osc.ch1.t_start.set(-10)
osc.ch1.t_stop.set(10)
osc.ch2.t_start.set(-10)
osc.ch2.t_stop.set(10)
osc.ch3.t_start.set(-10)
osc.ch3.t_stop.set(10)
osc.ch4.t_start.set(-10)
osc.ch4.t_stop.set(10)

amp = 1
PSG1.amp(amp)
PSG2.amp(amp)

station = Station()
station.snapshot()

station.add_component(osc)

# Criar um database
initialise_or_create_database_at("~/teste.db")

exp = load_or_create_experiment(experiment_name='osc outro dut 2',
                                sample_name="osc outro dut teste background")

def calculateGain():
    Y = osc.ch3.wavesample()
    n_mean = int(len(Y)/2)
    value = 2*np.mean(Y[n_mean:])
    value = 10*np.log(np.power(value,2))
    return value

gain = Parameter('gain',
                  label='gain',
                  unit='dB',
                  get_cmd=calculateGain)


# Medida de fato
meas = Measurement(exp=exp, station=station)

meas.register_parameter(PSG1.freq)
meas.register_parameter(gain, setpoints=[PSG1.freq])

with meas.run(write_in_background=True) as datasaver:
    for aFreq in np.linspace(1,500,500):
        time.delay(1)
        PSG1.freq(aFreq)
        PSG2.freq(aFreq)
        datasaver.add_result((gain, gain()), (PSG1.freq, PSG1.freq()))
