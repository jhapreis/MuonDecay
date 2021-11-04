#ifndef CFG_H
#define CFG_H

/**
 * File for the definitions and constants for the acquisition and analysis.
 * Change the values here and it'll take effect on the other scripts.
*/

#define NumberADChannels 2500 // Number of channels on the oscilloscope
#define FisrtPeak_MaxIndex 110 // Expected max-index to find a first-peak
#define TriggerInUnits 0 // Trigger value, in "oscilloscope units"
#define MaxTimeStampScope 10 // Value in micro-seconds

#define NumberBins_HistogramTimeDifference 100
#define LeftLimit_HistogramTimeDifference 0.1
#define RightLimit_HistogramTimeDifference 9.6


#endif //CFG_H
