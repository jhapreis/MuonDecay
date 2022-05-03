import os

import ROOT as root

import pandas as pd

from array import array

from modules.out_file.read_output_file import Get_AcquisitionParameters

from modules.out_file.convert_values import Convert_UnitsToMiliVolts_ScopeParameters


"""
Folder
    ROOT File
        TTree
            Branch
                Waveform
                    Point on the waveform (ADChannel)
"""



#====================================================================================================
def unify_data_files(
    
    path_to_folders:    "list[str]", 
    
    tree_name:          "str" = 'tree_waveforms',
        
    out_file:           "str" = 'output.txt',
    
    out_root_file_name: "str" = "muondecay.root",
    
    number_ADChannels:  "int" = 2500
    
    ) -> "int":
    

    #----------------------------------------------------------------------------------------------------
    event_name     = array('i', [0])
    
    waveform_in_mv = array('f', [0]*number_ADChannels)
    
    
    #----------------------------------------------------------------------------------------------------
    out_root_file  = root.TFile(out_root_file_name, "RECREATE")
    
    tree_waveforms = root.TTree(tree_name, "waveforms")
    
    tree_waveforms.Branch("names"    , event_name       , "name/I")
    
    tree_waveforms.Branch("waveforms", waveform_in_mv, f"waveforms[{number_ADChannels}]/F")
    
    
    #----------------------------------------------------------------------------------------------------    
    for folder in path_to_folders:

        print(f'\nfolder:  {folder}')


        df_output = Get_AcquisitionParameters(folder+'/'+out_file)
        
        root_files_in_folder = [(folder+_) for _ in os.listdir(folder) if _.endswith(".root")] 
        
        
        #----------------------------------------------------------------------------------------------------
        for file in root_files_in_folder:


            print(f'   file: {file}')
            

            #----------------------------------------------------------------------------------------------------
            f     = root.TFile.Open(file)
            
            tree  = f.Get(tree_name)
            
            wvfrm = array('i', [0]*number_ADChannels)
            
            name  = array('i', [0])
            
            tree.SetBranchAddress("waveforms", wvfrm)
            
            tree.SetBranchAddress("names"    , name )
            
            
            #----------------------------------------------------------------------------------------------------
            entries = tree.GetEntries()
            
            for event in range(entries):
                
                tree.GetEntry(event)

                for i in range(number_ADChannels):
                    
                    waveform_in_mv[i] = Convert_UnitsToMiliVolts_ScopeParameters(
                        y_units=wvfrm[i],
                        y_zero =df_output['y_zero'][0],
                        y_off  =df_output['y_off' ][0],
                        y_mult =df_output['y_mult'][0]
                    ) 
                
                event_name[0] = name[0]
                                        
                tree_waveforms.Fill()
            
            f.Close()
        
        
    
    
    #----------------------------------------------------------------------------------------------------
    out_root_file.Write()
    
    out_root_file.Close()
    
    
    
    return 0
