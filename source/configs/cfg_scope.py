'''
Oscilloscope ID from the TEKTRONIX TDS1002B
'''
ScopeID = 'USB0::0x0699::0x0363::C061073::INSTR'

'''
Parameters for the acquisition
'''
#minimal number of samples needed to retrieve
necessarySamples  = 100
#minimal number of samples per csv
samples           = 10
#number of random samples to collect
random_samples    = 100
#minimal amount of peaks; [1] for single muon and [2] for muon decay
min_peaks         = 1
#separation between the two peaks (1/2500 micro-seconds): optimal value = 60 ns
min_separation    = 150 
#
email_me          = True

'''
Parameters to set on the oscilloscope
'''
channel               = 'CH1'    #Sets or queries which waveform will be transferred from the oscilloscope by the queries. 
encode_format         = 'ASCII'  #Sets or queries the format of the waveform data. ASCII, binary etc.
width                 = 1        #Sets the data width to 1 byte per data point for CURVe data.
channel_scale         = 30.0E-3  #Valores permitidos: ]2mV, 5V[ ; V/div
channel_position      = 4.5      #previous: 2
channel_probe         = 1        #
trigger               = -10E-3   #Value in Volts; the values MUST be multiple of 0.6 mV
horizontal_scale      = 1.0E-6   #
horizontal_position_1 = 0        #Initial horizontal position 0 default
horizontal_position_2 = 4.6E-6   #Horizontal position with respect to the start of the oscilloscope window
persistence           = 'OFF'    #Persistence
slope                 = 'FALL'   #Negative slope
scopeResolution       = 2500     #número de pontos em cada waveform selecionada
numberBins            = 100      #number of bins to graph



