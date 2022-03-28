import ROOT as root

from array import array

import pandas as pd

import os



#====================================================================================================
def Convert_WaveformToMiliVolts(waveform_in_units, encoder, Scope_ChannelPosition, Scope_ChannelScale, Scope_NumberOfDisplayDivisions=10):
    
    """
    Converts Waveform in units to mV
    
        V(v) = Delta_V / Delta_v * (v - v_min) + V_min

    ----------
    Returns:
        array: waveform in mV
    """
    
    Position_min = (-1)*Scope_NumberOfDisplayDivisions / 2
    
    Position_max =      Scope_NumberOfDisplayDivisions / 2
    
    
    
    if(   encoder.upper() == "RPBINARY" ):
        units_min =    0
        units_max =  255    
        
    elif( encoder.upper() == "ASCII"    ):
        units_min = -127
        units_max =  128
        
    else:
        print(f"Erro no encoder: {encoder}\n\n")
        
        exit(1)
    
    ratio = (Position_max - Position_min) / (units_max - units_min)
    
        
    Waveform_mV = array('f', [0]*len(waveform_in_units))
    
    for i in range(len(waveform_in_units)):

        y_pos = ratio*(waveform_in_units[i] - units_min) + Position_min

        y_pos -= Scope_ChannelPosition                     # relative to Scope_ChannelPosition value

        Waveform_mV[i] = 1000 * Scope_ChannelScale * y_pos # converts from position to mV, using y-scale value (in volts)
    
    
    
    return Waveform_mV
    
    


"""
"""
#====================================================================================================
folder      = '../data/results/baseline/20220326_191818'

output_file = 'output.txt'

root_files  = [i for i in os.listdir(folder) if i.endswith(".root")]

tree_name   = 'tree_waveforms'


number_of_bins = 50

units_min      = 230

units_max      = 250




"""
Read output.txt
"""
#----------------------------------------------------------------------------------------------------
with open(folder+'/'+output_file, 'r') as f:
    
    lines = f.read()
    
    encoder                  = lines.split(   "DATA:ENCDG?,"   )[1].split("\n")[0]
    
    trigger_main_level       = float( lines.split(   "TRIGGER:MAIN:LEVEL?,"   )[1].split("\n")[0] )
    
    horizontal_main_scale    = float( lines.split(  "HORIZONTAL:MAIN:SCALE?," )[1].split("\n")[0] )
    
    horizontal_main_position = float( lines.split("HORIZONTAL:MAIN:POSITION?,")[1].split("\n")[0] )
    
    channel_scale            = float( lines.split(       "CH1:SCALE?,"        )[1].split("\n")[0] )
    
    channel_position         = float( lines.split(      "CH1:POSITION?,"      )[1].split("\n")[0] )
    
    channel_probe            = float( lines.split(       "CH1:PROBE?,"        )[1].split("\n")[0] )





"""
Add ROOT files to TChain

    Read the entire folder and add data to the TChain
"""
#----------------------------------------------------------------------------------------------------
chain = root.TChain(tree_name)

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




"""
Create histogram
"""
#----------------------------------------------------------------------------------------------------
hist_min, hist_max = Convert_WaveformToMiliVolts(
    array('i', [units_min, units_max]),
    encoder,
    channel_position,
    channel_scale
 )

hist = root.TH1F("hist", "Waveforms values", number_of_bins, hist_min, hist_max)



"""
Fit histogram with data
"""
#----------------------------------------------------------------------------------------------------
for i in range(entries):

    chain.GetEntry(i)

    waveform_in_mv = Convert_WaveformToMiliVolts(
        waveform_in_units,
        encoder,
        channel_position,
        channel_scale
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
