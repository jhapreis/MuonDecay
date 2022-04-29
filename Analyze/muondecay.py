import ROOT as root

from array import array

import matplotlib.pyplot as plt

import pandas as pd

import numpy as np

import sys

import os


from modules.convert_values import Convert_UnitsToMiliVolts_ScopeParameters, ConvertMiliVoltsToUnits_ScopeParameters, Convert_WaveformToMiliVolts_ScopeParameters, Convert_TimeToMicroSec

from modules.read_output_file import Get_AcquisitionParameters

from modules.find_peaks import FindPeaks_Waveform, Integral_Waveform

from modules.delete_files import delete_root_files_in_folder



def MuonDecay_Analysis(folder):

    #====================================================================================================
    number_of_bins = 50              
    pulsewidth     = 30              
    tree_name      = 'tree_waveforms'
    output_file    = 'output.txt'



    #----------------------------------------------------------------------------------------------------
    delete_root_files_in_folder(folder, tree_name)



    #----------------------------------------------------------------------------------------------------
    root_files  = [i for i in os.listdir(folder) if i.endswith(".root")]



    #----------------------------------------------------------------------------------------------------
    print(f'   Reading {output_file} ...')

    df_output        = Get_AcquisitionParameters(folder+'/'+output_file)

    trigger_in_mv    = 1000*df_output['trigger_main_level'][0]

    trigger_in_units = ConvertMiliVoltsToUnits_ScopeParameters(
        y_mv=trigger_in_mv,
        y_zero=df_output['y_zero'][0],
        y_off=df_output['y_off'][0],
        y_mult=df_output['y_mult'][0]
    )



    """
    Add ROOT files to TChain

        Read the entire folder and add data to the TChain
    """
    #----------------------------------------------------------------------------------------------------
    chain = root.TChain(tree_name)

    print('\n   Adding files to TChain...')

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
        
        chain.GetEntry(i)
        
        
        x_peaks = FindPeaks_Waveform(
            waveform_in_units, 
            trigger_in_units,
            pulsewidth,
            2
            )
        

        if len(x_peaks) != 2:
            
            errors.append([i, len(x_peaks)])
            
            continue
    
                
        time_diff = Convert_TimeToMicroSec(
            x_peaks[1] - x_peaks[0],
            df_output['horizontal_main_scale'][0]
            )
        
        time_differences.append(time_diff)
        
        
        y_peaks_0.append(
            -1*waveform_in_units[x_peaks[0]]
        )
        
        y_peaks_1.append(
            -1*waveform_in_units[x_peaks[1]]
            )
        
        
        integrals = Integral_Waveform(waveform_in_units, x_peaks=x_peaks, delta_x=1)

        integrals_0.append( integrals[0] )

        integrals_1.append( integrals[1] )

    print(f'   Error on FindPeaks: {len(errors)}\n')



    #----------------------------------------------------------------------------------------------------
    print('\n   Creating and filling histograms...')

    hist_time_difference = root.TH1F("hist_delta_t"   , "Time difference", number_of_bins, 0.5, 9 )

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

    exp_fit = root.TF1("exp_fit", "[0] + [1]*exp(-x/[2])", 0.5, 9)

    exp_fit.SetParNames("constant", "A", "tau")
    exp_fit.SetParameters(10, 100, 2)
    exp_fit.SetLineStyle(2)

    hist_time_difference.Fit("exp_fit")

    df = pd.DataFrame({
        'constant': [exp_fit.GetParameter(0)],
        'A'       : [exp_fit.GetParameter(1)],
        'tau'     : [exp_fit.GetParameter(2)]
    })

    df.index = ['values']

    df.T.to_csv(folder+'/expfit.csv')



    #----------------------------------------------------------------------------------------------------
    print('\n   Creating canvas')

    c1 = root.TCanvas("c1")
    c1.cd()
    hist_time_difference.GetXaxis().SetTitle("Time difference #mus")
    hist_time_difference.GetYaxis().SetTitle("counts")
    hist_time_difference.Draw()
    c1.SaveAs(folder+'/expfit.png')


    c2 = root.TCanvas("c2")
    c2.cd()
    hist_y_peaks_0.GetXaxis().SetTitle("Value")
    hist_y_peaks_0.GetYaxis().SetTitle("counts")
    hist_y_peaks_0.Draw()
    c2.SaveAs(folder+'/peaks_0.png')


    c3 = root.TCanvas("c3")
    c3.cd()
    hist_y_peaks_1.GetXaxis().SetTitle("Value")
    hist_y_peaks_1.GetYaxis().SetTitle("counts")
    hist_y_peaks_1.Draw()
    c3.SaveAs(folder+'/peaks_1.png')


    c4 = root.TCanvas("c4")
    c4.cd()
    hist_integral_0.GetXaxis().SetTitle("Integral")
    hist_integral_0.GetYaxis().SetTitle("counts")
    hist_integral_0.Draw()
    c4.SaveAs(folder+'/integral_0.png')


    c5 = root.TCanvas("c5")
    c5.cd()
    hist_integral_1.GetXaxis().SetTitle("Integral")
    hist_integral_1.GetYaxis().SetTitle("counts")
    hist_integral_1.Draw()
    c5.SaveAs(folder+'/integral_1.png')







if __name__ == "__main__":
    
    #----------------------------------------------------------------------------------------------------
    if len(sys.argv) <= 1:
        
        print("Error: you must pass the folder(s) path(es) as parameters when calling the script, but no argument was found.\n")
        
        exit(1)
    
    
    #----------------------------------------------------------------------------------------------------
    for folder in sys.argv[1:]:
        
        print(f"Folder: {folder}")
        
        MuonDecay_Analysis(folder)
