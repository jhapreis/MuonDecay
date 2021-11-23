#ifndef CFG_H
#define CFG_H

/**
 * File for the definitions and constants for the acquisition and analysis.
 * Change the values here and it'll take effect on the other scripts.
*/



/**
 * Acquisition parameters.
 * Those are the software parameters. 
 */
#define FisrtPeak_MaxIndex 110                // Expected max-index to find a first-peak
#define TriggerInUnits 0                      // Trigger value, in "oscilloscope units"
#define MaxTimeStampScope 10                  // Value in micro-seconds
#define Acquisition_NecessarySamples 100      // Minimal number of samples needed to retrieve
#define Acquisition_NumberOfSamplesBuffer 100 // Number of random samples to collect
#define Acquisition_MinPeaks 1                // 



/**
 * Scope Parameters. 
 * Set on the scope hardware, using NI-VISA libraries. 
 */
#define USB_Instrument "USB0::0x0699::0x0363::04WRL8::INSTR" // USB oscilloscope ID
#define Scope_TimeOutValue 3000                              // 
#define Scope_NumberADChannels 2500                          // Number of channels on the oscilloscope
#define Scope_DataEncodeFormat "RPBinary"
#define Scope_DataEncodeWidth 1
#define Scope_NumberOfBins 100
#define Scope_Persistence "OFF"
#define Scope_ChannelTriggerSlope "FALL"
#define Scope_ChannelName "CH1"
#define Scope_ChannelScale 30E-3
#define Scope_ChannelPosition 4.5
#define Scope_ChannelProbe 1
#define Scope_ChannelTrigger -10E-3
#define Scope_ChannelHorizontalScale 1E-6
#define Scope_ChannelHorizontalPosition 4.6E-6
#define Scope_NumberOfDisplayDivisions 10                    // Immutable; from scope, phisically



/**
 * 
 * 
 */
#define NumberBins_HistogramTimeDifference 100
#define LeftLimit_HistogramTimeDifference 0
#define RightLimit_HistogramTimeDifference 10



#endif //CFG_H
