import ROOT as root

from array import array

import pandas as pd

import os


from modules.convert_values import Convert_WaveformToMiliVolts, Convert_TimeToMicroSec

from modules.read_output_file import Get_AcquisitionParameters

from modules.find_peaks import FindPeaks_Waveform, Integral_Waveform

from modules.fit_muon_decay import A_ExpX_C



"""
"""
#====================================================================================================
folder      = '../data/results/baseline/20220326_191818'

output_file = 'output.txt'

root_files  = [i for i in os.listdir(folder) if i.endswith(".root")]

tree_name   = 'tree_waveforms'


number_of_bins = 50

units_min      = 230

units_max      = 250

pulsewidth     = 100



df_output = Get_AcquisitionParameters(folder+'/'+output_file)



"""
Add ROOT files to TChain

    Read the entire folder and add data to the TChain
"""
#----------------------------------------------------------------------------------------------------
chain = root.TChain(tree_name)

for i in range( len(root_files) ): #len(root_files)
    
    file = folder+'/'+root_files[i]
        
    chain.Add(file)




"""
Assign array to be filled with the .root file data
"""
#----------------------------------------------------------------------------------------------------
waveform_in_units = array('i', [0]*2500)

chain.SetBranchAddress("waveforms", waveform_in_units)

entries = chain.GetEntries()



#----------------------------------------------------------------------------------------------------
hist_time_difference = root.TH1F("hist", "Time difference", number_of_bins, 0, 255)

hist_y_peaks_0       = root.TH1F("hist", "y_peaks_0"      , number_of_bins, 0, 255)

hist_y_peaks_1       = root.TH1F("hist", "y_peaks_1"      , number_of_bins, 0, 255)

hist_integral_0      = root.TH1F("hist", "integral_0"     , number_of_bins, 0, 255)

hist_integral_1      = root.TH1F("hist", "integral_1"     , number_of_bins, 0, 255)



#----------------------------------------------------------------------------------------------------
for i in range(entries):
    
    chain.GetEntry(i)
    
    
    x_peaks = FindPeaks_Waveform(
        waveform_in_units, 
        df_output['trigger_main_level'][0],
        pulsewidth,
        2
        )
    
    if not x_peaks: continue
    
    
    y_peaks = [
        waveform_in_units[ x_peaks[0] ], 
        waveform_in_units[ x_peaks[1] ]
        ]
    
    
    time_difference = Convert_TimeToMicroSec(x_peaks[1] - x_peaks[0])
    
    # integrals = Integral_Waveform(waveform_in_units)
    
    
    hist_time_difference.Fill(time_difference)
    
    hist_y_peaks_0.Fill(y_peaks[0])
    
    hist_y_peaks_1.Fill(y_peaks[1])
    
    # hist_integral_0.Fill(integrals[0])
    
    # hist_integral_1.Fill(integrals[1])


c1 = root.TCanvas()

hist_time_difference.GetXaxis().SetTitle("Time difference #mus")
hist_time_difference.GetYaxis().SetTitle("counts")

hist_time_difference.Draw()



#----------------------------------------------------------------------------------------------------
exp_fit = root.TF1("exp_fit", A_ExpX_C, 0, 255)

hist_time_difference.Fit("exp_fit")
