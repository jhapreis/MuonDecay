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
    
    path_to_folders:     "list[str]", 

    tree_name:           "str" = 'tree_waveforms',
        
    out_file:            "str" = 'output.txt',
    
    outroot_folder_path: "str" = './',

    outroot_file_path:   "str" = "./muondecay.root",
    
    number_ADChannels:   "int" = 2500
    
    ) -> "int":
    

    #----------------------------------------------------------------------------------------------------
    event_name     = array('i', [0])
    
    waveform_in_mv = array('f', [0]*number_ADChannels)
    
    
    #----------------------------------------------------------------------------------------------------
    out_root_file  = root.TFile(outroot_file_path, "RECREATE")
    
    tree_waveforms = root.TTree(tree_name, "waveforms")
    
    tree_waveforms.Branch("names"    , event_name       , "name/I")
    
    tree_waveforms.Branch("waveforms", waveform_in_mv, f"waveforms[{number_ADChannels}]/F")
    
    df = pd.DataFrame()
    
    
    #----------------------------------------------------------------------------------------------------    
    for folder in path_to_folders:

        print(f'   folder:  {folder}')


        df_output = Get_AcquisitionParameters(folder+'/'+out_file)
        
        df        = pd.concat([df, df_output], axis=0)
                
        
        root_files_in_folder = [(folder+_) for _ in os.listdir(folder) if _.endswith(".root")] 
        
        
        #----------------------------------------------------------------------------------------------------        
        chain = root.TChain(tree_name)
        
        for file in root_files_in_folder:
            
            chain.Add(file)
            
        wvfrm = array('i', [0]*number_ADChannels)
            
        name  = array('i', [0])
        
        chain.SetBranchAddress("waveforms", wvfrm)
        
        chain.SetBranchAddress("names"    , name )
        
            
        #----------------------------------------------------------------------------------------------------
        entries = chain.GetEntries()
                    
        for i in range(entries):
            
            chain.GetEntry(i)
            
            for j in range(number_ADChannels):
                
                event_name[0] = name[0]
                
                waveform_in_mv[j] = wvfrm[j]
                
                '''
                O gargalo da função está aqui, nesse pedaço, que eu não consegui simplificar
                '''
                waveform_in_mv[j] = Convert_UnitsToMiliVolts_ScopeParameters(
                    y_units=wvfrm[j],
                    y_zero =df_output['y_zero'][0],
                    y_off  =df_output['y_off' ][0],
                    y_mult =df_output['y_mult'][0]
                )
            
            tree_waveforms.Fill()
        
        
    
    
    #----------------------------------------------------------------------------------------------------
    out_root_file.Write()
    
    out_root_file.Close()
    
    df.index = [str(i) for i in range(df.shape[0])]
    
    df.T.to_csv(outroot_folder_path+'/output.csv')
    
    
    return 0
