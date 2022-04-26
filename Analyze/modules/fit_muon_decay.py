import pandas as pd

import numpy as np



def A_ExpX_C(xdata, A, tau, C):
    
    ydata = A*np.exp(-xdata/tau) + C
    
    return ydata



def Exponential_Fit():
    
    pass



def NumberBins_Sturge(number_of_samples):
    
    number_bins = round( 1 + 3.322*np.log10(number_of_samples) )
    
    return number_bins
