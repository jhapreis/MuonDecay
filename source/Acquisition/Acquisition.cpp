/**
 * @file acquisition.cpp
 * @brief 
 * @version 0.1
 * @date 2021-11-23
 * 
 * 
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

#include "Configs/cfg.h"
#include "includes/acquisition.h"
#include "includes/peaks.h"



int main(){

    
    TStopwatch time_elapsed;    // Measure elapsed time
    
    char err[32];               // Error string
    
    char root_FileName[64];     // ROOT file name as time epoch

    int numberGoodSamples = 0;  // Number of good samples collected until then

    
    int status_waveform = 0;    // Waveform curve, as an event

    int WaveformAsInt[Scope_NumberADChannels];

    memset(WaveformAsInt, Acquisition_WaveformInitializer, sizeof(WaveformAsInt));


    
    int numberPeaksWaveform = 0;// Number of peaks found on the waveform


    double triggerUnits = Convert_VoltsToUnits(Scope_ChannelTrigger); // Trigger, converted from Volts to Units
    
    printf("\n   Trigger in units = %f", triggerUnits);


    // Scope parameters
    ViSession rm          = VI_NULL;         // Resource Manager
    ViSession scope       = VI_NULL;         // Oscilloscope
    ViStatus status_scope = VI_NULL;         // failure or success
    ViUInt32 retCount     = VI_NULL;         // retCount
    ViChar buffer[4*Scope_NumberADChannels]; // Buffer; size > Scope_NumberADChannels



    /**
     * @brief ROOT FILE AND TTREES
     *  
     * Create ROOT file and set the TTree to store the 
     * required waveforms. 
     */

    int event_name = 0;

    char waveformsArrayROOT[20];

    sprintf(waveformsArrayROOT, "waveform[%d]/I", Scope_NumberADChannels);


    sprintf(root_FileName, "../data/%d_%lu.root", Acquisition_NecessarySamples, time(NULL));

    TFile* root_file = new TFile(root_FileName, "CREATE");


    TTree* tree_scope_infos = new TTree("tree_infos", "infos");

    TTree* tree_waveforms   = new TTree("tree_waveforms", "waveforms");

    tree_waveforms->Branch("names", &event_name, "name/I");
    
    tree_waveforms->Branch("waveforms", WaveformAsInt, "waveforms[2500]/I");
    


    /**
     * @brief CONNECT TO OSCILLOSCOPE
     *  
     * Open a default session and the USB device.
     * Handling situations in case of error or in case of success.
     */

    status_scope = viOpenDefaultRM(&rm);
    status_scope = viOpen(rm, USB_Instrument, VI_NULL, VI_NULL, &scope);

    if(status_scope < VI_SUCCESS){
        strcpy(err, "Error opening device");
        goto error;
    }
    printf("\n\n\n      Oscilloscope RM: open.\n");



    /**
     * @brief SET SCOPE PARAMETERS 
     * 
     * Run a SetScopeParameters function in order to prepare for
     * acquisition. Saves the informations on the ROOT file.
     * After that, tries to read an IDN query. If failed, exits.
     *  
     */
    printf("\n      Setting Scope Parameters...\n");
    Set_ScopeParameters(status_scope, scope, retCount, tree_scope_infos);

    status_scope = viWrite(scope, (ViBuf) "*IDN?\n"     , 6             , &retCount);
    status_scope = viRead( scope, (ViBuf) buffer        , sizeof(buffer), &retCount);
    if(status_scope < VI_SUCCESS){
        strcpy(err, "Error opening device");
        goto error;
    }
    printf("ID: %s\n\n      ...done\n\n", buffer);



    /**
     * @brief ACQUISITION 
     * 
     * The following block of code stands for the acquisition.
     * It will fill a buffer-array-like with data (maybe good, maybe not so good).
     * After the buffer is filled, the code is going to evaluate the data and 
     * (a) save on the ROOT TTree or (b) discard the event.
     * 
     * -- While numberGoodSamples < NecessarySamples, search for a new sample.
     * -- Fill buffer
     * -- Analyze from buffer
     * -- Save good events
     * -- Repeat
     */

    printf("      Collecting data... %d events necessary. Min peaks = %d\n\n", Acquisition_NecessarySamples, Acquisition_MinPeaks);

    while(numberGoodSamples < Acquisition_NecessarySamples){


        // Resets buffer
        memset(buffer, 0, sizeof(buffer));
  

        // Query for curve and get the epoch time as event_name
        status_scope = viWrite(scope, (ViBuf) "CURVE?\n", 7             , &retCount);
        status_scope = viRead( scope, (ViBuf) buffer    , sizeof(buffer), &retCount);
        if( status_scope < VI_SUCCESS ){
            printf("error on query: status < VI_SUCCESS: \n");
            continue;
        }
        event_name = (int) time(NULL);


        // Unpacking curve into int array
        status_waveform = Get_CurveData(buffer, WaveformAsInt);
        if( status_waveform < 0 ){
            printf("error on query: waveform unpacking error: \n");
            continue;
        }
        
        
        // Count the number of peaks on the waveform
        numberPeaksWaveform = SearchPeaksNumber(WaveformAsInt, Scope_NumberADChannels, triggerUnits, Acquisition_MinPeaks);
        if(numberPeaksWaveform < 0) continue; // if not successfull 


        /**
         * @brief SAVING DATA 
         *    If no errors, then we got a good event. We create a new branch 
         * on the TTree and name it as the number of the event.
         *    After that, we read the current waveform and store the values 
         * on the ROOT file (point to point).
         *    Then, free the current waveform data.
         */        

        tree_waveforms->Fill();
        
        numberGoodSamples++;
    }



    /**
     * @brief Finish execution 
     */

    printf("\n      Finishing...\n\n");

    time_elapsed.Print();
    printf("\n\n");

    root_file->Write();
    root_file->Close();

    GraphWaveforms(root_FileName);

    return 0;
    
    

    
    error:
        // Report error and clean up
        viStatusDesc(scope, status_scope, buffer);
        fprintf(stderr, "\n       %s: failure: %s\n", err, buffer);
        if(rm != VI_NULL) viClose(rm);
        root_file->Write();
        root_file->Close();
        return 1;  
}
