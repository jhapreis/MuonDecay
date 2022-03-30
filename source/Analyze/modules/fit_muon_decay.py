import pandas as pd

import numpy as np



def A_ExpX_C(xdata, A, tau, C):
    
    ydata = A*np.exp(-xdata/tau) + C
    
    return ydata



def Exponential_Fit():
    
    pass
