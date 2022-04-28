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

        else:
            
            index += 1
            
    
    
    return x_coordinate_peaks
                


#====================================================================================================
def Integral_Waveform(waveform, x_peaks, delta_x, pulsewidth=30):
    
    Integrals = []

    baseline = np.mean(waveform)


    for i in range(len(x_peaks)):
        
        left_lim  = max( 0   , int(x_peaks[i] - pulsewidth/2) ) 

        right_lim = min( 2500, int(x_peaks[i] + pulsewidth/2) )

        waveform_pulse = np.array(waveform[left_lim:right_lim]) - baseline
        

        integral = sum(waveform_pulse) * delta_x


        Integrals.append(integral)


    return np.array(Integrals)


