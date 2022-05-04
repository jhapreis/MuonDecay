import ROOT as root

from array import array

import matplotlib.pyplot as plt

import pandas as pd

import numpy as np

import sys

import os


from modules.out_file.convert_values import Convert_UnitsToMiliVolts_ScopeParameters, ConvertMiliVoltsToUnits_ScopeParameters, Convert_WaveformToMiliVolts_ScopeParameters, Convert_TimeToMicroSec

from modules.out_file.read_output_file import Get_AcquisitionParameters

from modules.fit.find_peaks import FindPeaks_Waveform, Integral_Waveform



#====================================================================================================
def MuonDecay_Analysis(
    
    root_file_path:   "str" = './muondecay.root',
    
    results_path:     "str" = './results',
    
    tree_name:        "str" = "tree_waveforms",
    
    number_of_bins:   "int" = 50, 
            
    pulsewidth:       "int" = 30,
        
    output_file_path: "str" ='./output.csv'
    
    ) -> "int":


    os.makedirs(results_path, exist_ok=True)


    #----------------------------------------------------------------------------------------------------
    df = pd.read_csv(output_file_path, index_col=0).T

    trigger_in_mv    = 1000*float( df['trigger_main_level'][0] )
    
    print(trigger_in_mv)
    


    #----------------------------------------------------------------------------------------------------
    file    = root.TFile.Open(root_file_path)
    
    tree    = file.Get(tree_name)
    
    waveform_in_mv = array('f', [0]*2500)

    tree.SetBranchAddress("waveforms", waveform_in_mv)
    
    entries = tree.GetEntries() 
    
    print(f'\n   Number of entries: {entries}')    



    #----------------------------------------------------------------------------------------------------
    print('\n   Finding peaks...')

    errors           = []

    time_differences = []

    y_peaks_0        = []

    y_peaks_1        = []

    integrals_0      = []

    integrals_1      = []

    for i in range(entries):
        
        tree.GetEntry(i)
        
        
        x_peaks = FindPeaks_Waveform(
            waveform                 = waveform_in_mv, 
            height                   = trigger_in_mv,
            max_pulse_width          = pulsewidth,
            expected_number_of_peaks = 2
            )
        

        if len(x_peaks) != 2:
            
            errors.append([i, len(x_peaks)])
            
            continue
    
                
        time_diff = Convert_TimeToMicroSec(
            x_peaks[1] - x_peaks[0],
            df['horizontal_main_scale'][0]
            )
        
        time_differences.append(time_diff)
        
        
        y_peaks_0.append(
            -1*waveform_in_mv[x_peaks[0]]
        )
        
        y_peaks_1.append(
            -1*waveform_in_mv[x_peaks[1]]
            )
        
        
        integrals = Integral_Waveform(waveform_in_mv, x_peaks=x_peaks, delta_x=1)

        integrals_0.append( integrals[0] )

        integrals_1.append( integrals[1] )

    print(f'   Error on FindPeaks: {len(errors)}\n')



    #----------------------------------------------------------------------------------------------------
    print('\n   Creating and filling histograms...')

    hist_time_difference = root.TH1F("hist_delta_t"   , "Time difference", number_of_bins, 0.8, 9.2 )

    hist_y_peaks_0       = root.TH1F("hist_y_peaks_0" , "y_peaks_0"      , number_of_bins, -255  , 0 )

    hist_y_peaks_1       = root.TH1F("hist_y_peaks_1" , "y_peaks_1"      , number_of_bins, -255  , 0  )

    hist_integral_0      = root.TH1F("hist_integral_0", "integral_0"     , number_of_bins, min(integrals_0), max(integrals_0) )

    hist_integral_1      = root.TH1F("hist_integral_1", "integral_1"     , number_of_bins, min(integrals_1), max(integrals_1) )


    for i in range(len(time_differences)):
        
        hist_time_difference.Fill(time_differences[i])
        
        hist_y_peaks_0.Fill(y_peaks_0[i])
        
        hist_y_peaks_1.Fill(y_peaks_1[i])
        
        hist_integral_0.Fill(integrals_0[i])
        
        hist_integral_1.Fill(integrals_1[i])



    #----------------------------------------------------------------------------------------------------
    print('\n   Fit exponential\n')

    exp_fit = root.TF1("exp_fit", "[0] + [1]*exp(-x/[2])", 0.8, 9.2)

    exp_fit.SetParNames("constant", "A", "tau")
    exp_fit.SetParameters(10, 100, 2)
    exp_fit.SetLineStyle(2)

    hist_time_difference.Fit("exp_fit")

    df = pd.DataFrame({
        'constant': [exp_fit.GetParameter(0)],
        'A'       : [exp_fit.GetParameter(1)],
        'tau'     : [exp_fit.GetParameter(2)]
        })

    df.reset_index(inplace=True)
    
    df.index = ['parameter', 'value']

    df.T.to_csv(results_path+'/expfit.csv')



    #----------------------------------------------------------------------------------------------------
    print('\n   Creating canvas')

    c1 = root.TCanvas("c1")
    c1.cd()
    hist_time_difference.GetXaxis().SetTitle("Time difference #mus")
    hist_time_difference.GetYaxis().SetTitle("counts")
    hist_time_difference.Draw()
    c1.SaveAs(results_path+'/expfit.png')


    c2 = root.TCanvas("c2")
    c2.cd()
    hist_y_peaks_0.GetXaxis().SetTitle("Value")
    hist_y_peaks_0.GetYaxis().SetTitle("counts")
    hist_y_peaks_0.Draw()
    c2.SaveAs(results_path+'/peaks_0.png')


    c3 = root.TCanvas("c3")
    c3.cd()
    hist_y_peaks_1.GetXaxis().SetTitle("Value")
    hist_y_peaks_1.GetYaxis().SetTitle("counts")
    hist_y_peaks_1.Draw()
    c3.SaveAs(results_path+'/peaks_1.png')


    c4 = root.TCanvas("c4")
    c4.cd()
    hist_integral_0.GetXaxis().SetTitle("Integral")
    hist_integral_0.GetYaxis().SetTitle("counts")
    hist_integral_0.Draw()
    c4.SaveAs(results_path+'/integral_0.png')


    c5 = root.TCanvas("c5")
    c5.cd()
    hist_integral_1.GetXaxis().SetTitle("Integral")
    hist_integral_1.GetYaxis().SetTitle("counts")
    hist_integral_1.Draw()
    c5.SaveAs(results_path+'/integral_1.png')
    
    
    
    return 0







if __name__ == "__main__":
    
    #----------------------------------------------------------------------------------------------------
    if len(sys.argv) <= 1:
        
        print("Error: you must pass the folder(s) path(es) as parameters when calling the script, but no argument was found.\n")
        
        exit(1)
    
    
    #----------------------------------------------------------------------------------------------------
    for folder in sys.argv[1:]:
        
        print(f"Folder: {folder}")
        
        MuonDecay_Analysis(
            root_file_path   = f'{folder}/muondecay/muondecay.root',
            results_path     = f'{folder}/results',
            tree_name        = 'tree_waveforms',
            number_of_bins   = 50,
            pulsewidth       = 30,
            output_file_path = f'{folder}/muondecay/output.csv'
        )
