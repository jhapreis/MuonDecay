import os

import ROOT as root

import matplotlib.pyplot as plt

from array import array



#====================================================================================================
def GraphWaveforms_Folder(

    folder_path: "str", 

    numberADChannels: "int" = 2500, 

    tree_name: "str" = "tree_waveforms", 
    
    branch_name: "str" = "waveforms", 
    
    output_file: "str" = "output.txt"
    
    ) -> "int":
        

    #----------------------------------------------------------------------------------------------------
    root_files = [i for i in os.listdir(folder_path) if i.endswith(".root")]
    
    
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
    
    fig, ax = plt.subplots()
    
    ax.set_title(f'{entries} waveforms')
    ax.set_xlabel(f'time (units)')
    ax.set_ylabel(f'value')
    
    for i in range(entries):
        
        chain.GetEntry(i)
        
        ax.plot(waveform_in_units, color='black')
    
    
    fig.savefig(folder_path+'/waveforms.png')
    
    
    return 0





#====================================================================================================
def GraphWaveforms_File(

    file_name:        "str", 

    folder_path:      "str"= "./",

    numberADChannels: "int"=2500, 

    tree_name:        "str"="tree_waveforms", 
    
    branch_name:      "str"="waveforms" 
    
    ) -> "int":



    #----------------------------------------------------------------------------------------------------
    file = root.TFile.Open(file_name)

    tree = file.Get(tree_name)



    #----------------------------------------------------------------------------------------------------
    waveform_in_mv = array('f', [0]*numberADChannels)

    tree.SetBranchAddress(branch_name, waveform_in_mv)

    entries = tree.GetEntries()



    # Draw waveforms
    #----------------------------------------------------------------------------------------------------
    
    fig, ax = plt.subplots()
    
    ax.set_title(f'{entries} waveforms')
    ax.set_xlabel(f'time (ADChannel)')
    ax.set_ylabel(f'value (mV)')
    
    for i in range(entries):
        
        tree.GetEntry(i)
        
        ax.plot(waveform_in_mv, color='black')
    
    
    fig.savefig(folder_path+'/waveforms.png')

    plt.clf()



    return 0
