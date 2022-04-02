/**
 * @file acquisition.cpp
 * @brief 
 * @version 0.2
 * @date 2022-04-02
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <ctime>

#include <visa.h>

#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TGraph.h>
#include <TImage.h>
#include <TLatex.h>
#include <TStopwatch.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "../Configs/cfg.h"
#include "includes/acquisition.h"
#include "includes/peaks.h"



int main(int argc, char **argv){

    // Declaring variables
    //----------------------------------------------------------------------------------------------------
    TStopwatch timeElapsed;         // Measure elapsed time
    
    char err[32];                   // Error string

    char root_FileName[64];         // ROOT file name, filled as time epoch

    int startTime             = 0;  // Time at the beggining of the run, as time epoch 

    int nowTime               = 0;  // Time now, as time epoch

    int numberGoodSamples     = 0;  // Number of good samples collected until then

    int numberPeaksWaveform   = 0;  // Number of peaks found on the waveform
    
    int status_waveform       = 0;  // Waveform curve, as an event

    int event_name            = 0;  // Name of the event, as time epoch

    double triggerUnits       = 0;  // Trigger, converted from Volts to Units

    TFile* root_file        = NULL; // ROOT file

    TTree* tree_waveforms   = NULL; // TTree on the ROOT file 


    int WaveformAsInt[Scope_NumberADChannels]; // Array to store the waveform as Int values

    memset(WaveformAsInt, Acquisition_WaveformInitializer, sizeof(WaveformAsInt));  



    //----------------------------------------------------------------------------------------------------
    // Start timer
    //----------------------------------------------------------------------------------------------------
    timeElapsed.Start();



    //----------------------------------------------------------------------------------------------------
    // Convert trigger value from Volts to units
    //----------------------------------------------------------------------------------------------------
    triggerUnits = Convert_VoltsToUnits(Scope_ChannelTrigger); 
    
    printf("\n   Trigger in units = %f", triggerUnits);



    //----------------------------------------------------------------------------------------------------
    // Scope parameters
    //----------------------------------------------------------------------------------------------------
    ViSession rm          = VI_NULL;         // Resource Manager
    ViSession scope       = VI_NULL;         // Oscilloscope
    ViStatus status_scope = VI_NULL;         // failure or success
    ViUInt32 retCount     = VI_NULL;         // retCount
    ViChar buffer[4*Scope_NumberADChannels]; // Buffer; size > Scope_NumberADChannels
    


    //----------------------------------------------------------------------------------------------------
    /**
     * @brief CONNECT TO OSCILLOSCOPE
     *  
     * Open a default session and the USB device.
     * Handling situations in case of error or in case of success.
     */
    //----------------------------------------------------------------------------------------------------
    status_scope = viOpenDefaultRM(&rm);
    status_scope = viOpen(rm, USB_Instrument, VI_NULL, VI_NULL, &scope);

    if(status_scope < VI_SUCCESS){
        strcpy(err, "Error opening device");
        goto error;
    }



    //----------------------------------------------------------------------------------------------------
    /**
     * @brief ACQUISITION 
     * 
     * The following block of code stands for the acquisition.
     * It will fill a buffer-array-like with data (maybe good, maybe not so good).
     * After the buffer is filled, the code is going to evaluate the data and 
     * (a) save on the ROOT TTree or (b) discard the event.
     * 
     * -- While (...).
     * -- Fill buffer
     * -- Analyze from buffer
     * -- Save good events
     * -- Repeat
     */
    //----------------------------------------------------------------------------------------------------
    printf("      Collecting data... Min peaks = %d\n\n", Acquisition_MinPeaks);

    startTime = (int) std::time(nullptr);

    nowTime   = (int) std::time(nullptr);

    while(1){

        // If the time difference is greater than the maximum for a file, close the file and renew the cicle
        //----------------------------------------------------------------------------------------------------
        if(nowTime - startTime >= Acquisition_RunTimeMaximum){

            printf("\n      Closing file...\n");

            timeElapsed.Print();
            printf("\n\n");

            if(root_file){
                root_file->Write();
                root_file->Close();
            }

            startTime = (int) std::time(nullptr);

            continue;
        }


        // Create ROOT file and set the TTree to store the required waveforms.
        //----------------------------------------------------------------------------------------------------
        sprintf(root_FileName, "%s/%ld.root", argv[1], std::time(nullptr));

        root_file      = new TFile(root_FileName   , "CREATE");
        tree_waveforms = new TTree("tree_waveforms", "waveforms");
        tree_waveforms->Branch("names"    , &event_name  , "name/I");
        tree_waveforms->Branch("waveforms", WaveformAsInt, "waveforms[2500]/I");


        // Resets buffer
        //----------------------------------------------------------------------------------------------------
        memset(buffer, 0, sizeof(buffer));
  

        // Query for curve and get the epoch time as event_name
        //----------------------------------------------------------------------------------------------------
        status_scope = viWrite(scope, (ViBuf) "CURVE?\n", 7             , &retCount);
        status_scope = viRead( scope, (ViBuf) buffer    , sizeof(buffer), &retCount);
        if( status_scope < VI_SUCCESS ){
            printf("error on query: status < VI_SUCCESS: \n");
            continue;
        }
        event_name = (int) std::time(nullptr);


        // Unpacking curve into int array
        //----------------------------------------------------------------------------------------------------
        status_waveform = Get_CurveData(buffer, WaveformAsInt);
        if( status_waveform < 0 ){      // erro no salvamento da waveform
            continue;
        }
        else if( status_waveform == 1){ // erro na descompactação da waveform, exit
            printf("error on query: waveform unpacking error: \n"); 
            exit(1);
        }
        
        
        // Count the number of peaks on the waveform
        //----------------------------------------------------------------------------------------------------
        numberPeaksWaveform = SearchPeaksNumber(WaveformAsInt, Scope_NumberADChannels, triggerUnits, Acquisition_MinPeaks);
        if(numberPeaksWaveform < 0) continue; // if not successfull 


        /**
         * @brief SAVING DATA 
         *    If no errors, then we got a good event. We create a new branch 
         * on the TTree and name it as the number of the event.
         *    After that, we read the current waveform and store the values 
         * on the ROOT file (point to point).
         */        

        tree_waveforms->Fill();
        
        numberGoodSamples++;

        nowTime = (int) std::time(nullptr);
    }

    return 1;



    //----------------------------------------------------------------------------------------------------
    error: // Report error and clean up
        timeElapsed.Stop();
        timeElapsed.Print();

        viStatusDesc(scope, status_scope, buffer);
        fprintf(stderr, "\n       %s: failure: %s\n", err, buffer);
        if(rm != VI_NULL) viClose(rm);
        root_file->Write();
        root_file->Close();

        return 1;  
}
