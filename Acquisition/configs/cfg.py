"""
File for the definitions and constants for the acquisition and analysis.
Change the values here and it'll take effect on the other scripts.
"""



"""
Acquisition parameters. Those are the software parameters. 
"""

MaxTimeStampScope = 10                  # Value in micro-seconds
Acquisition_MinSeparation = 10          # Value in units on the x-axis
Acquisition_PulseWidth = 30             # Value supposed to be the pulse width
Acquisition_WaveformInitializer = -1    # 
Acquisition_RunTimeMaximum = 3600*6     # Max time of the file, in seconds
Acquisition_MinPeaks = 2                # 



"""
Scope Parameters. 
Set on the scope hardware, using NI-VISA libraries. 
"""

USB_Instrument = "USB0::0x0699::0x0363::04WRL8::INSTR" # USB oscilloscope ID
Scope_TimeOutValue = 3000                              # 
Scope_NumberADChannels = 2500                          # Number of channels on the oscilloscope
Scope_DataEncodeFormat = "RPBinary"
Scope_DataEncodeWidth = 1
Scope_NumberOfBins = 100
Scope_Persistence = "OFF"
Scope_ChannelTriggerSlope = "FALL"
Scope_ChannelName = "CH1"
Scope_ChannelPosition = 4.5
Scope_ChannelProbe = 1
Scope_ChannelHorizontalScale = 1E-6
Scope_ChannelHorizontalPosition = 4.2E-6
Scope_NumberOfDisplayDivisions = 10                   # Immutable; from scope, phisically
Scope_ChannelTrigger = -60E-3
Scope_ChannelScale = 100E-3                    
