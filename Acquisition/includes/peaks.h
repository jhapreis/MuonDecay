#ifndef PEAKS_H
#define PEAKS_H

#include "../configs/cfg.h"



//====================================================================================================
/**
 * @brief Gets a str* and makes it to UPPERCASE.
 * It does change the original char*.
 * 
 * @param str 
 * @return char* 
 */
char* uppercase(char* str);



//====================================================================================================
/**
 * @brief 
 * 
 * @param Waveform_Units 
 * @param numberPoitsWaveform 
 * @return double* 
 */
double* Convert_WaveformMiliVolts(int* Waveform_Units, int numberPoitsWaveform);



//====================================================================================================
/**
 * @brief 
 * 
 * @param valueVolts 
 * @return double 
 */
double Convert_VoltsToUnits(double valueVolts);



//====================================================================================================
/**
 * @brief 
 * 
 * @param waveformAsInt 
 * @param waveformSize 
 * @param height 
 * @param expectedPeaks 
 * @return int 
 */
int SearchPeaksNumber(int* waveformAsInt, int waveformSize, double height, int expectedPeaks);



#endif
