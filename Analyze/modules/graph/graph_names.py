import ROOT as root

from array import array

import seaborn as sns

import matplotlib.pyplot as plt

import pandas as pd


#====================================================================================================
def GraphIncidence_File(

    file_name:        "str", 

    folder_path:      "str"= "./",

    tree_name:        "str"="tree_waveforms", 
    
    branch_name:      "str"="names",

    number_of_bins:   "int"=50 
    
    ) -> "int":



    #----------------------------------------------------------------------------------------------------
    file = root.TFile.Open(file_name)

    tree = file.Get(tree_name)



    #----------------------------------------------------------------------------------------------------
    name = array('i', [0])

    tree.SetBranchAddress(branch_name, name)

    entries = tree.GetEntries()


    times = []


    for i in range(entries):
        
        tree.GetEntry(i)

        times.append(name[0])
    
    times = pd.DataFrame(times, columns=['time'])



    # Draw waveforms
    #----------------------------------------------------------------------------------------------------    
    sns.histplot(data=times, bins=number_of_bins)

    
    plt.savefig(folder_path+'/incidence.png')

    plt.clf()


    file.Close()



    return 0
