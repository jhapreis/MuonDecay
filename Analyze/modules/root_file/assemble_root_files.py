import os

import ROOT as root

import pandas as pd

from array import array

from modules.out_file.read_output_file import Get_AcquisitionParameters



#====================================================================================================
def unify_data_files(
    
    path_to_folders: "list[str]", 
    
    tree_name: "str"          = 'tree_waveforms',
        
    out_file:  "str"          = 'output.txt',
    
    out_root_file_name: "str" = "muondecay.root",
    
    number_ADChannels: "int"  = 2500
    
    ):
    
    #----------------------------------------------------------------------------------------------------
    event_name        = array('i', [0])
    
    waveform_in_units = array('i', [0]*number_ADChannels)
    
    
    #----------------------------------------------------------------------------------------------------
    out_root_file  = root.TFile(out_root_file_name, "RECREATE")
    
    tree_waveforms = root.TTree(tree_name, "waveforms")
    
    tree_waveforms.Branch("names"    , event_name       , "name/I")
    
    tree_waveforms.Branch("waveforms", waveform_in_units, f"waveforms[{number_ADChannels}]/I")
    
    
    #----------------------------------------------------------------------------------------------------
    root_files_paths = root_files_names(path_to_folders=path_to_folders) 
    
    for file in root_files_paths:
        
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
                
                waveform_in_units[i] = wvfrm[i]
            
            event_name[0] = name[0]
                                    
            tree_waveforms.Fill()
        
        f.Close()
        
        
    
    
    #----------------------------------------------------------------------------------------------------
    out_root_file.Write()
    
    out_root_file.Close()
    
    
    
    return 0



#====================================================================================================
def root_files_names(path_to_folders: "list[str]") -> "list[str]":
    
    root_files = []
    
    
    for folder in path_to_folders:
        
        root_files_in_folder = [(folder+_) for _ in os.listdir(folder) if _.endswith(".root")]
        
        root_files.extend(root_files_in_folder)
        
    
    return root_files
