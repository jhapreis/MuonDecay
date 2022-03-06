#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <ctype.h>

#include <TTree.h>

#include <visa.h>

#include "../includes/acquisition.h"
#include "../../Configs/cfg.h"



//====================================================================================================

int Set_ScopeParameters(ViStatus status, ViSession scope, ViUInt32 retCount, TTree* tree){

    /**
     * @brief For the manteiner:
     * If you want to add another parameter to save on the ROOT file,
     * just copy the "paragraph" method and change the correspondent parameters.
     * 
     * Pay attention: if the tree value is NULL, then the code only do the 
     * writing/setup methods and do not even arrive on the query part.
     * 
     * TIMEOUT
     * DATA:SOURCE
     * DATa:ENCdg
     * DATA:WIDTH
     * TRIGGER:MAIN:LEVEL
     * TRIGGER:MAIN:EDGE:SLOPE
     * HORIZONTAL:MAIN:SCALE
     * HORIZONTAL:MAIN:POSITION
     * DISPLAY:PERSISTENCE
     * CH<X>:SCALE
     * CH<X>:POSITION
     * CH<X>:PROBE
     * 
     * 
     */

    char cmd[64], buffer[256];
    std::string str;
    TBranch* b = NULL;


    // Set Time Out
    status = viSetAttribute(scope, VI_ATTR_TMO_VALUE, Scope_TimeOutValue);
    if(status < VI_SUCCESS) goto error;

    
    // Set Channel
    sprintf(cmd, "DATA:SOURCE %s", Scope_ChannelName);
    status = viWrite(scope, (ViBuf) cmd, strlen(cmd), &retCount);
    if(status < VI_SUCCESS) goto error;

    
    // Set Encoding
    sprintf(cmd, "DATa:ENCdg %s", Scope_DataEncodeFormat);
    status = viWrite(scope, (ViBuf) cmd, strlen(cmd), &retCount);
    if(status < VI_SUCCESS) goto error;
    

    // Set Data Width
    sprintf(cmd, "DATA:WIDTH %d", Scope_DataEncodeWidth);
    status = viWrite(scope, (ViBuf) cmd, strlen(cmd), &retCount);
    if(status < VI_SUCCESS) goto error;

    
    // Set Trigger Level
    sprintf(cmd, "TRIGGER:MAIN:LEVEL %f", Scope_ChannelTrigger);
    status = viWrite(scope, (ViBuf) cmd, strlen(cmd), &retCount);
    if(status < VI_SUCCESS) goto error;

    
    // Set Trigger Slope
    sprintf(cmd, "TRIGGER:MAIN:EDGE:SLOPE %s", Scope_ChannelTriggerSlope);
    status = viWrite(scope, (ViBuf) cmd, strlen(cmd), &retCount);
    if(status < VI_SUCCESS) goto error;

    
    // Set x-axis scale
    sprintf(cmd, "HORIZONTAL:MAIN:SCALE %f", Scope_ChannelHorizontalScale);
    status = viWrite(scope, (ViBuf) cmd, strlen(cmd), &retCount);
    if(status < VI_SUCCESS) goto error;

    
    // Set x-axis "center"
    sprintf(cmd, "HORIZONTAL:MAIN:POSITION %f", Scope_ChannelHorizontalPosition);
    status = viWrite(scope, (ViBuf) cmd, strlen(cmd), &retCount);
    if(status < VI_SUCCESS) goto error;

    
    // Set Persistence
    sprintf(cmd, "DISPLAY:PERSISTENCE %s", Scope_Persistence);
    status = viWrite(scope, (ViBuf) cmd, strlen(cmd), &retCount);
    if(status < VI_SUCCESS) goto error;

    
    // Set Channel Scale
    sprintf(cmd, "%s:SCALE %f", Scope_ChannelName, Scope_ChannelScale);
    status = viWrite(scope, (ViBuf) cmd, strlen(cmd), &retCount);
    if(status < VI_SUCCESS) goto error;

    
    // Set Channel Position
    sprintf(cmd, "%s:POSITION %f", Scope_ChannelName, Scope_ChannelPosition);
    status = viWrite(scope, (ViBuf) cmd, strlen(cmd), &retCount);
    if(status < VI_SUCCESS) goto error;

    
    // Set Channel Probe
    sprintf(cmd, "%s:PROBE %d", Scope_ChannelName, Scope_ChannelProbe);
    status = viWrite(scope, (ViBuf) cmd, strlen(cmd), &retCount);
    if(status < VI_SUCCESS) goto error;

    

    // Check if it was required to save the infos on a ROOT file
    if(tree == NULL) return 0;



    /**
     * QUERY INFOS FROM SCOPE 
     */

    memset(buffer, 0, sizeof(buffer));
    memset(cmd   , 0, sizeof(cmd));


    sprintf(cmd, "DATA:SOURCE?");
    status = viWrite(scope, (ViBuf) cmd   , strlen(cmd)   , &retCount);
    status = viRead( scope, (ViBuf) buffer, sizeof(buffer), &retCount);
    str = std::string(buffer);
    b = tree->Branch(cmd, &str); 
    b->Fill();   
    printf("DATA:SOURCE? %s \n", buffer);
    memset(buffer, 0, sizeof(buffer));
    memset(cmd   , 0, sizeof(cmd));
    

    sprintf(cmd, "DATA:ENCDG?");
    status = viWrite(scope, (ViBuf) cmd   , strlen(cmd)   , &retCount);
    status = viRead( scope, (ViBuf) buffer, sizeof(buffer), &retCount);
    str = std::string(buffer);
    b = tree->Branch(cmd, &str); 
    b->Fill();   
    printf("DATA:ENCDG? %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));
    memset(cmd   , 0, sizeof(cmd));


    sprintf(cmd, "DATA:WIDTH?");
    status = viWrite(scope, (ViBuf) cmd   , strlen(cmd)   , &retCount);
    status = viRead( scope, (ViBuf) buffer, sizeof(buffer), &retCount);
    str = std::string(buffer);
    b = tree->Branch(cmd, &str); 
    b->Fill();   
    printf("DATA:WIDTH? %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));
    

    sprintf(cmd, "TRIGGER:MAIN:LEVEL?");
    status = viWrite(scope, (ViBuf) cmd   , strlen(cmd)   , &retCount);
    status = viRead( scope, (ViBuf) buffer, sizeof(buffer), &retCount);
    str = std::string(buffer);
    b = tree->Branch(cmd, &str); 
    b->Fill();   
    printf("TRIGGER:MAIN:LEVEL? %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));
    memset(cmd   , 0, sizeof(cmd));
    

    sprintf(cmd, "TRIGGER:MAIN:EDGE:SLOPE?");
    status = viWrite(scope, (ViBuf) cmd   , strlen(cmd)   , &retCount);
    status = viRead( scope, (ViBuf) buffer, sizeof(buffer), &retCount);
    str = std::string(buffer);
    b = tree->Branch(cmd, &str); 
    b->Fill();   
    printf("TRIGGER:MAIN:EDGE:SLOPE? %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));
    memset(cmd   , 0, sizeof(cmd));


    sprintf(cmd, "HORIZONTAL:MAIN:SCALE?");
    status = viWrite(scope, (ViBuf) cmd   , strlen(cmd)   , &retCount);
    status = viRead( scope, (ViBuf) buffer, sizeof(buffer), &retCount);
    str = std::string(buffer);
    b = tree->Branch(cmd, &str); 
    b->Fill();   
    printf("HORIZONTAL:MAIN:SCALE? %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));
    memset(cmd   , 0, sizeof(cmd));


    sprintf(cmd, "HORIZONTAL:MAIN:POSITION?");
    status = viWrite(scope, (ViBuf) cmd   , strlen(cmd)   , &retCount);
    status = viRead( scope, (ViBuf) buffer, sizeof(buffer), &retCount);
    str = std::string(buffer);
    b = tree->Branch(cmd, &str); 
    b->Fill();   
    printf("HORIZONTAL:MAIN:POSITION? %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));
    memset(cmd   , 0, sizeof(cmd));


    sprintf(cmd, "DISPLAY:PERSISTENCE?");
    status = viWrite(scope, (ViBuf) cmd   , strlen(cmd)   , &retCount);
    status = viRead( scope, (ViBuf) buffer, sizeof(buffer), &retCount);
    str = std::string(buffer);
    b = tree->Branch(cmd, &str); 
    b->Fill();   
    printf("DISPLAY:PERSISTENCE? %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));
    memset(cmd   , 0, sizeof(cmd));


    sprintf(cmd, "%s:SCALE?", Scope_ChannelName);
    status = viWrite(scope, (ViBuf) cmd   , strlen(cmd)   , &retCount);
    status = viRead( scope, (ViBuf) buffer, sizeof(buffer), &retCount);
    str = std::string(buffer);
    b = tree->Branch(cmd, &str); 
    b->Fill();   
    printf("%s:SCALE? %s\n", Scope_ChannelName, buffer);
    memset(buffer, 0, sizeof(buffer));
    memset(cmd   , 0, sizeof(cmd));


    sprintf(cmd, "%s:POSITION?", Scope_ChannelName);
    status = viWrite(scope, (ViBuf) cmd   , strlen(cmd)   , &retCount);
    status = viRead( scope, (ViBuf) buffer, sizeof(buffer), &retCount);
    str = std::string(buffer);
    b = tree->Branch(cmd, &str); 
    b->Fill();   
    printf("%s:POSITION? %s\n", Scope_ChannelName, buffer);
    memset(buffer, 0, sizeof(buffer));
    memset(cmd   , 0, sizeof(cmd));


    sprintf(cmd, "%s:PROBE?", Scope_ChannelName);
    status = viWrite(scope, (ViBuf) cmd   , strlen(cmd)   , &retCount);
    status = viRead( scope, (ViBuf) buffer, sizeof(buffer), &retCount);
    str = std::string(buffer);
    b = tree->Branch(cmd, &str); 
    b->Fill();   
    printf("%s:PROBE? %s\n", Scope_ChannelName, buffer);
    memset(buffer, 0, sizeof(buffer));
    memset(cmd   , 0, sizeof(cmd));


    sprintf(cmd, "WFMPRE?");
    status = viWrite(scope, (ViBuf) cmd   , strlen(cmd)   , &retCount);
    status = viRead( scope, (ViBuf) buffer, sizeof(buffer), &retCount);
    str = std::string(buffer);
    b = tree->Branch(cmd, &str); 
    b->Fill();   
    printf("WFMPRE?\n   %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));   
    memset(cmd   , 0, sizeof(cmd)); 


    sprintf(cmd, "MIN_PEAKS");
    sprintf(buffer, "%d", Acquisition_MinPeaks);
    str = std::string(buffer);
    b = tree->Branch(cmd, &str); 
    b->Fill();   
    memset(buffer, 0, sizeof(buffer));   
    memset(cmd   , 0, sizeof(cmd)); 


    sprintf(cmd, "NECESSARY_SAMPLES");
    sprintf(buffer, "%d", Acquisition_NecessarySamples);
    str = std::string(buffer);
    b = tree->Branch(cmd, &str); 
    b->Fill();   
    memset(buffer, 0, sizeof(buffer));   
    memset(cmd   , 0, sizeof(cmd));


    sprintf(cmd, "NUMBER_ADCHANNELS");
    sprintf(buffer, "%d", Scope_NumberADChannels);
    str = std::string(buffer);
    b = tree->Branch(cmd, &str); 
    b->Fill();   
    memset(buffer, 0, sizeof(buffer));   
    memset(cmd   , 0, sizeof(cmd));


    sprintf(cmd, "PULSE_WIDTH");
    sprintf(buffer, "%d", Acquisition_PulseWidth);
    str = std::string(buffer);
    b = tree->Branch(cmd, &str); 
    b->Fill();   
    memset(buffer, 0, sizeof(buffer));   
    memset(cmd   , 0, sizeof(cmd));



    return 0;



    error:
        // Report error and clean up
        viStatusDesc(scope, status, buffer);
        fprintf(stderr, "\n       failure: %s\n", buffer);
        return 1;
}