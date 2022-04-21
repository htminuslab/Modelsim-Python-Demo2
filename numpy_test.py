#---------------------------------------------------------------------------------------------------
# Simple FLI demo calling the Python Numpy/Matplotlib libraries
#
#  https://github.com/htminuslab   
#---------------------------------------------------------------------------------------------------
#
#---------------------------------------------------------------------------------------------------
# Update: Created 30/11/2017
# Update: Added matplotlib 04/12/2018
#       : Must use TK version from Python and not the (default) one from Modelsim. This can
#       : be done by setting the TCLLIBPATH env variable before starting Modelsim.
# Update: Updated for Python 3.8 and Modelsim 2022.1 14/03/2022
#---------------------------------------------------------------------------------------------------
import numpy as np
import matplotlib.pyplot as plt

def do_fft(*x):

    print("Calculate FFT uvsimsing Numpy",np.__version__)   # Print Numpy version used

    nbsamples=len(*x)
    samplingfrequency = 500;                                # 500 hz sampling
    
    fhat=np.fft.fft(*x)
    fhat2=fhat[0:int(nbsamples/2)]                          # use range 0 to sampling_frequency/2
    sampletime=nbsamples/samplingfrequency                  # 500/100=5 seconds
    xfreq=np.arange(int(nbsamples/2))/sampletime    

    print("length",len(xfreq), len(fhat2), len(fhat))

    fig,axs=plt.subplots(2, 1)                              # Plot input signal from VHDL
    plt.sca(axs[0])
    plt.plot(*x,color='g',label='Input')
    plt.grid()
    plt.legend()     
    plt.sca(axs[1])
    plt.plot(xfreq,abs(fhat2),color='b',label='FFT')
    plt.grid()
    plt.legend() 
    plt.show()
    
    return (abs(fhat)).tolist()                             # Return FFT back to VHDL
    