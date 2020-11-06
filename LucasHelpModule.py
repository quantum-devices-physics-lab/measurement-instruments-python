import matplotlib.pyplot as plt
import numpy as np
import scipy.interpolate as spy


# Wrap phase______________
def unwrap_d(Phase):
    """
    Unwrap Phase in Degree"""
    return np.unwrap(Phase * np.pi / 180)


def wrap(Phase):
    """ Wrap back the phase 
    """
    return (Phase + np.pi) % (2 * np.pi) - np.pi


def wrap1(Phase):
    """ Wrap back the phase using arctan2
    """
    return np.arctan2(np.sin(Phase), np.cos(Phase))

    


def dBmtoV(Amp):
    """Convert dBm to Volts
    Input Amplitude in dBm
    """
    c = 10 * np.log10(20)
    return np.power(10, (Amp - c) / 20)


def VtodBm(V):
    """Convert Volts to dBm
    Input Voltage in Volts
    """
    c = 10 * np.log10(20)
    return 20 * np.log10(V) + c


# Convert dBm to W

def dBmtoW(Amp):
    """Convert dBm to Watts
    Imput Amp in dBm
    """
    return np.power(10, (Amp - 30) / 10)


def fit_peak(X,Y,n):
    from scipy.optimize import curve_fit
    def func(x, a, b, c):
        return a*(x-b)**2+c

    peak = np.where(Y == np.min(Y))[0][0]
    begin = peak-n
    end = peak +n
    n_interpolate = 41
    xnew = np.linspace(X[begin], X[end-1], num=n_interpolate, endpoint=True)
    popt, pcov = curve_fit(func, X[begin:end], Y[begin:end],p0=[1,X[peak],Y[peak]])
    ynew= func(xnew,*popt)
    
    return xnew,ynew

def S2PParser(filename):
    '''S2P files parser. Useful to parse the data returned by our network analyser'''
    file = open(filename,"r")
    lines = file.readlines()
    file.close()
    info = ''.join(lines[:5])
    
    Freqs = []
    S11s_dB = []
    S11s_angle = []
    S21s_dB = []
    S21s_angle = []
    S12s_dB = []
    S12s_angle = []
    S22s_dB = []
    S22s_angle = []


    for line in lines[5:]:
        data = line.split('\n')[0].split('\t')

        Freqs.append(float(data[0]))

        S11s_dB.append(float(data[1]))
        S11s_angle.append(float(data[2]))

        S21s_dB.append(float(data[3]))
        S21s_angle.append(float(data[4]))

        S12s_dB.append(float(data[5]))
        S12s_angle.append(float(data[6]))

        S22s_dB.append(float(data[7]))
        S22s_angle.append(float(data[8]))
    
    return (info,Freqs,S11s_dB,S11s_angle,S21s_dB,S21s_angle,S12s_dB,S12s_angle,S22s_dB,S22s_angle)

def bigplot(X,Y,xlabel,ylabel,title):
    '''Helper function to get east bigplots'''
    fig,ax = plt.subplots(figsize=(14,7))
    ax.plot(X,Y,linewidth=3)
    ax.set_xlabel(xlabel,fontsize=20)
    ax.set_ylabel(ylabel,fontsize=20)

    ax.tick_params(axis='x', labelsize=20)
    ax.tick_params(axis='y', labelsize=20)

    ax.set_title(title,fontsize=20)
    
    return fig,ax