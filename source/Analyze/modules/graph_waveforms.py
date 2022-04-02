import os

import ROOT as root

import pandas as pd

from array import array


from modules.delete_files import delete_root_files_in_folder

from modules.read_output_file import Get_AcquisitionParameters



#====================================================================================================
def GraphWaveforms_Folder(folder_path, numberADChannels=2500, tree_name="tree_waveforms", branch_name="waveforms", output_file="output.txt"):
    
    
    #----------------------------------------------------------------------------------------------------
    delete_root_files_in_folder(folder_path, tree_name)
    

    #----------------------------------------------------------------------------------------------------
    root_files = [i for i in os.listdir(folder_path) if i.endswith(".root")]
    
    
    #----------------------------------------------------------------------------------------------------
    # df_output = Get_AcquisitionParameters(folder_path+'/'+output_file)
    
    
    # Fill TChain
    #----------------------------------------------------------------------------------------------------
    chain = root.TChain(tree_name)

    for i in range( len(root_files) ):
        
        file = folder_path+'/'+root_files[i]
        
        chain.Add(file)  


    #----------------------------------------------------------------------------------------------------
    waveform_in_units = array('i', [0]*numberADChannels)

    chain.SetBranchAddress(branch_name, waveform_in_units)

    entries = chain.GetEntries()
    
    
    # Draw waveforms
    #----------------------------------------------------------------------------------------------------
    
    c1 = root.TCanvas()
    graph = root.TGraph()
    graph.SetTitle(f"{entries} Waveforms; time (units); value")
    # graph.SetLineWidth(2)
    # graph.SetLineColor("black")
    
    
    index = 0
    
    event = 0
    
    while event < entries:
        
        chain.GetEntry(event)
    
        for i in range(numberADChannels):
            
            graph.SetPoint(index, i, waveform_in_units[i])
            
            index += 1
            
        graph.Draw("AL")
        
        event += 1
    
    
    c1.SaveAs(folder_path+'/waveforms.png')
    
    
    return 0
