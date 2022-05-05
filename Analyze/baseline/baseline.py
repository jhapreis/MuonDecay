import ROOT as root

from array import array

import pandas as pd

import os


from modules.convert_values import Convert_WaveformToMiliVolts

from modules.read_output_file import Get_AcquisitionParameters

from modules.delete_files import delete_root_files_in_folder





"""
"""
#====================================================================================================
folder      = '../data/results/baseline/20220326_191818'

output_file = 'output.txt'

tree_name   = 'tree_waveforms'

delete_root_files_in_folder(folder, tree_name)

root_files  = [i for i in os.listdir(folder) if i.endswith(".root")]


number_of_bins = 100

units_min      = 230

units_max      = 250



df_output = Get_AcquisitionParameters(folder+'/'+output_file)



"""
Add ROOT files to TChain

    Read the entire folder and add data to the TChain
"""
#----------------------------------------------------------------------------------------------------
chain = root.TChain(tree_name)

print('\nAdding files')

for i in range( min(10,len(root_files))  ): #
    
    file = folder+'/'+root_files[i]
    
    chain.Add(file)  

print('...   done')



"""
Assign array to be filled with the .root file data
"""
#----------------------------------------------------------------------------------------------------
waveform_in_units = array('i', [0]*2500)

chain.SetBranchAddress("waveforms", waveform_in_units)

entries = chain.GetEntries()




"""
Create histogram
"""
#----------------------------------------------------------------------------------------------------
hist_min, hist_max = Convert_WaveformToMiliVolts(
    array('i', [units_min, units_max]),
    df_output['encoder'][0]           ,
    df_output['channel_position'][0]  ,
    df_output['channel_scale'][0]
 )

hist = root.TH1F("hist", "Waveforms values", number_of_bins, hist_min, hist_max)



"""
Fit histogram with data
"""
#----------------------------------------------------------------------------------------------------
for i in range(entries):

    chain.GetEntry(i)

    waveform_in_mv = Convert_WaveformToMiliVolts(
        waveform_in_units               ,
        df_output['encoder'][0]         ,
        df_output['channel_position'][0],
        df_output['channel_scale'][0]
        )

    for j in range(2500):

        hist.Fill(waveform_in_mv[j])
        
        
c1 = root.TCanvas()

hist.GetXaxis().SetTitle("waveform ADChannel (mV)")
hist.GetYaxis().SetTitle("counts")

hist.Draw()



"""
Fit histogram with data
"""
#----------------------------------------------------------------------------------------------------
fit = root.TF1("fit", "gaus", hist_min, hist_max)

hist.Fit("fit", "R")

df = pd.DataFrame({
    'constant': [fit.GetParameter(0)],
    'mean'    : [fit.GetParameter(1)],
    'sigma'   : [fit.GetParameter(2)]
})

df.index = ['values']

df.T.to_csv(folder+'/gausfit.csv')

c1.SaveAs(folder+'/baseline.png')

lower = df['mean'] - 3*df['sigma']

upper = df['mean'] + 3*df['sigma']

print(f"\n\n3 sigmas de distancia: {lower[0]} mV, {upper[0]} mV\n")
