import ROOT as root

from array import array

import pandas as pd

import os


from modules.convert_values import Convert_WaveformToMiliVolts, Convert_TimeToMicroSec

from modules.read_output_file import Get_AcquisitionParameters

from modules.find_peaks import FindPeaks_Waveform, Integral_Waveform

from modules.fit_muon_decay import A_ExpX_C

from modules.delete_files import delete_root_files_in_folder




"""
"""
#====================================================================================================
folder      = '../data/20220327_001241'

output_file = 'output.txt'

tree_name   = 'tree_waveforms'

delete_root_files_in_folder(folder, tree_name)

root_files  = [i for i in os.listdir(folder) if i.endswith(".root")]



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

print('\nAdding files')

for i in range( len(root_files) ): #len(root_files)
    
    file = folder+'/'+root_files[i]
    
    chain.Add(file)  

print('...   done')



"""
Assign array to be filled with the .root file data
"""
#----------------------------------------------------------------------------------------------------
waveform_in_units = array('i', [0]*2500)

chain.SetBranchAddress("waveforms", waveform_in_units)

entries = chain.GetEntries()

print(f'\nNumber of entries: {entries}')



#----------------------------------------------------------------------------------------------------
print('\nCreating histograms...')

hist_time_difference = root.TH1F("hist_delta_t"   , "Time difference", number_of_bins, 0, 10 )

hist_y_peaks_0       = root.TH1F("hist_y_peaks_0" , "y_peaks_0"      , number_of_bins, 0, 255)

hist_y_peaks_1       = root.TH1F("hist_y_peaks_1" , "y_peaks_1"      , number_of_bins, 0, 255)

hist_integral_0      = root.TH1F("hist_integral_0", "integral_0"     , number_of_bins, 0, 255)

hist_integral_1      = root.TH1F("hist_integral_1", "integral_1"     , number_of_bins, 0, 255)

print('...   done')



#----------------------------------------------------------------------------------------------------
print('\nFinding peaks and filling histograms')

errors = 0

time_differences = []

for i in range(entries):
    
    chain.GetEntry(i)
    
    waveform_in_mv = Convert_WaveformToMiliVolts(
        waveform_in_units,
        df_output['encoder'][0],
        df_output['channel_position'][0],
        df_output['channel_scale'][0]
    )
    
    x_peaks = FindPeaks_Waveform(
        waveform_in_mv, 
        1000*df_output['trigger_main_level'][0],
        pulsewidth,
        2
        )
    
    if not x_peaks:
        
        errors += 1
        
        continue
    
    
    y_peaks = [
        waveform_in_mv[ x_peaks[0] ], 
        waveform_in_mv[ x_peaks[1] ]
        ]
    
    
    time_difference = Convert_TimeToMicroSec(x_peaks[1] - x_peaks[0])
    
    time_differences.append(time_difference)
    
    # integrals = Integral_Waveform(waveform_in_units)
    
    
    hist_time_difference.Fill(time_difference)
    
    hist_y_peaks_0.Fill(y_peaks[0])
    
    hist_y_peaks_1.Fill(y_peaks[1])
    
    # hist_integral_0.Fill(integrals[0])
    
    # hist_integral_1.Fill(integrals[1])

print(f'Error on FindPeaks: {errors}\n')



#----------------------------------------------------------------------------------------------------
print('\nCreating canvas')

c1 = root.TCanvas()

hist_time_difference.GetXaxis().SetTitle("Time difference #mus")
hist_time_difference.GetYaxis().SetTitle("counts")

hist_time_difference.Draw()



#----------------------------------------------------------------------------------------------------
print('\nFit exponential\n')

exp_fit = root.TF1("exp_fit", "[0] + [1]*exp(-x/[2])", 0, 10)

exp_fit.SetParNames("constant", "A", "tau")
exp_fit.SetParameters(200, 2, 3)
exp_fit.SetLineStyle(2)

hist_time_difference.Fit("exp_fit")

df = pd.DataFrame({
    'constant': [exp_fit.GetParameter(0)],
    'A'       : [exp_fit.GetParameter(1)],
    'tau'     : [exp_fit.GetParameter(2)]
})

df.index = ['values']

df.T.to_csv(folder+'/expfit.csv')

c1.SaveAs(folder+'/expfit.png')
