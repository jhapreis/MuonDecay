import os

import ROOT as root

import matplotlib.pyplot as plt

from array import array


from modules.read_output_file import Get_AcquisitionParameters



#====================================================================================================
def GraphWaveforms_Folder(folder_path, numberADChannels=2500, tree_name="tree_waveforms", branch_name="waveforms", output_file="output.txt"):
        

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
    
    fig, ax = plt.subplots()
    
    ax.set_title(f'{entries} waveforms')
    ax.set_xlabel(f'time (units)')
    ax.set_ylabel(f'value')
    
    for i in range(entries):
        
        chain.GetEntry(i)
        
        ax.plot(waveform_in_units, color='black')
    
    
    fig.savefig(folder_path+'/waveforms.png')
    
    
    return 0
