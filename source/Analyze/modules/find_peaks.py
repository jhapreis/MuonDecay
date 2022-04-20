import pandas as pd

import numpy as np



#====================================================================================================
def FindPeaks_Waveform(waveform, height, max_pulse_width, expected_number_of_peaks):
    
    
    x_peak       = 0
    
    y_peak       = 0
    
    number_peaks = 0
    
    x_coordinate_peaks = []
    
    index = 0
    
    while index < len(waveform):

        if( waveform[index] <= height ): # if triggered
            
            x_peak = index
            y_peak = waveform[index]
                                    
            for j in range(index, min(index+max_pulse_width, len(waveform))): # find min on region
                                
                if(waveform[j] < y_peak):
                    
                    x_peak = j
                    
                    y_peak = waveform[j]
                    
            
            x_coordinate_peaks.append(x_peak)
            
            index += max_pulse_width
            
            number_peaks += 1

        index += 1
            
    
    if number_peaks != expected_number_of_peaks:
        
        return [-1, number_peaks]
    
    
    return x_coordinate_peaks
                


#====================================================================================================
def Integral_Waveform(waveform):
    
    pass
