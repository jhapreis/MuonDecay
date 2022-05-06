import ROOT as root

from array import array

import pandas as pd

import os


from modules.out_file.convert_values import Convert_TimeToMicroSec

from modules.fit.find_peaks import FindPeaks_Waveform, Integral_Waveform



#====================================================================================================
def MuonDecay_Analysis(
    
    root_file_path:   "str" = './muondecay.root',
    
    results_path:     "str" = './results',
    
    tree_name:        "str" = "tree_waveforms",
            
    pulsewidth:       "int" = 30,
        
    output_file_path: "str" ='./output.csv'
    
    ) -> "int":



    #----------------------------------------------------------------------------------------------------
    os.makedirs(results_path, exist_ok=True)


    time_difference = array('f', [0])

    x_peak_0        = array('f', [0])

    x_peak_1        = array('f', [0])

    y_peak_0        = array('f', [0])

    y_peak_1        = array('f', [0])

    integral_0      = array('f', [0])

    integral_1      = array('f', [0])


    results_file = root.TFile(results_path+'/results.root', 'RECREATE')

    results_tree = root.TTree('results', 'results')

    results_tree.Branch('time_difference', time_difference, 'time_difference/F')

    results_tree.Branch('x_peak_0'       , x_peak_0       , 'x_peak_0/F'       )

    results_tree.Branch('x_peak_1'       , x_peak_1       , 'x_peak_1/F'       )

    results_tree.Branch('y_peak_0'       , y_peak_0       , 'y_peak_0/F'       )

    results_tree.Branch('y_peak_1'       , y_peak_1       , 'y_peak_1/F'       )

    results_tree.Branch('integral_0'     , integral_0     , 'integral_0/F'     )

    results_tree.Branch('integral_1'     , integral_1     , 'integral_1/F'     )



    #----------------------------------------------------------------------------------------------------
    df_output     = pd.read_csv(output_file_path, index_col=0).T

    trigger_in_mv = 1000*float( df_output['trigger_main_level'][0] )
    
    print(f'   \nTrigger = {trigger_in_mv} mV')
    


    #----------------------------------------------------------------------------------------------------
    data_file      = root.TFile.Open(root_file_path)
    
    data_tree      = data_file.Get(tree_name)
    
    waveform_in_mv = array('f', [0]*2500)

    data_tree.SetBranchAddress("waveforms", waveform_in_mv)  



    #----------------------------------------------------------------------------------------------------
    print('\n   Finding peaks...')

    errors  = []
    
    entries = data_tree.GetEntries() 
    
    print(f'\n   Number of entries: {entries}')  

    for i in range(entries):


        #----------------------------------------------------------------------------------------------------
        data_tree.GetEntry(i)
        
        
        #----------------------------------------------------------------------------------------------------
        x_peaks = FindPeaks_Waveform(
            waveform                 = waveform_in_mv, 
            height                   = trigger_in_mv,
            max_pulse_width          = pulsewidth,
            expected_number_of_peaks = 2
            )
        

        #----------------------------------------------------------------------------------------------------
        if len(x_peaks) != 2:
            
            errors.append([i, len(x_peaks)])
            
            continue

        
        #----------------------------------------------------------------------------------------------------
        x_peak_0[0] = Convert_TimeToMicroSec(
            x_peaks[0],
            float(df_output['horizontal_main_scale'][0])
            )

        x_peak_1[0] = Convert_TimeToMicroSec(
            x_peaks[1],
            float(df_output['horizontal_main_scale'][0])
            )
        
        time_difference[0] = x_peak_1[0] - x_peak_0[0]
        
        
        y_peak_0[0] = waveform_in_mv[x_peaks[0]]
        
        y_peak_1[0] = waveform_in_mv[x_peaks[1]]
        
        
        integrals = Integral_Waveform(waveform_in_mv, x_peaks=x_peaks, delta_x=1)

        integral_0[0] = integrals[0]

        integral_1[0] = integrals[1]


        #----------------------------------------------------------------------------------------------------
        results_tree.Fill()



    #----------------------------------------------------------------------------------------------------
    print(f'   Error on FindPeaks: {len(errors)}\n')

    results_file.Write()

    results_file.Close()


    return 0
