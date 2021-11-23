#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <ctype.h>

#include <visa.h>

#include "../configs/cfg.h"
#include "acquisition.h"



//==================================================

int Set_ScopeParameters(ViStatus status, ViSession scope, ViUInt32 retCount){

    char cmd[64], buffer[256];


    // Set Time Out
    status = viSetAttribute(scope, VI_ATTR_TMO_VALUE, Scope_TimeOutValue);
    if(status < VI_SUCCESS) goto error;

    
    // Set Channel
    sprintf(cmd, "DATA:SOURCE %s", Scope_ChannelName);
    status = viWrite(scope, (ViBuf) cmd, strlen(cmd), &retCount);
    if(status < VI_SUCCESS) goto error;

    
    // Set Channel
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

    



    /**
     * QUERY INFOS FROM SCOPE 
    **/

    printf("\n");
    memset(buffer, 0, sizeof(buffer));
    

    sprintf(cmd, "DATA:SOURCE?\n");
    status = viWrite(scope, (ViBuf) cmd   , strlen(cmd)   , &retCount);
    status = viRead( scope, (ViBuf) buffer, sizeof(buffer), &retCount);
    printf("DATA:SOURCE? %s \n", buffer);
    memset(buffer, 0, sizeof(buffer));
    

    sprintf(cmd, "DATA:ENCDG?\n");
    status = viWrite(scope, (ViBuf) cmd   , strlen(cmd)   , &retCount);
    status = viRead( scope, (ViBuf) buffer, sizeof(buffer), &retCount);
    printf("DATA:ENCDG? %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));


    sprintf(cmd, "DATA:WIDTH?\n");
    status = viWrite(scope, (ViBuf) cmd   , strlen(cmd)   , &retCount);
    status = viRead( scope, (ViBuf) buffer, sizeof(buffer), &retCount);
    printf("DATA:WIDTH? %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));
    

    sprintf(cmd, "TRIGGER:MAIN:LEVEL?\n");
    status = viWrite(scope, (ViBuf) cmd   , strlen(cmd)   , &retCount);
    status = viRead( scope, (ViBuf) buffer, sizeof(buffer), &retCount);
    printf("TRIGGER:MAIN:LEVEL? %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));
    

    sprintf(cmd, "TRIGGER:MAIN:EDGE:SLOPE?\n");
    status = viWrite(scope, (ViBuf) cmd   , strlen(cmd)   , &retCount);
    status = viRead( scope, (ViBuf) buffer, sizeof(buffer), &retCount);
    printf("TRIGGER:MAIN:EDGE:SLOPE? %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));


    sprintf(cmd, "HORIZONTAL:MAIN:SCALE?\n");
    status = viWrite(scope, (ViBuf) cmd   , strlen(cmd)   , &retCount);
    status = viRead( scope, (ViBuf) buffer, sizeof(buffer), &retCount);
    printf("HORIZONTAL:MAIN:SCALE? %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));


    sprintf(cmd, "HORIZONTAL:MAIN:POSITION?\n");
    status = viWrite(scope, (ViBuf) cmd   , strlen(cmd)   , &retCount);
    status = viRead( scope, (ViBuf) buffer, sizeof(buffer), &retCount);
    printf("HORIZONTAL:MAIN:POSITION? %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));


    sprintf(cmd, "DISPLAY:PERSISTENCE?\n");
    status = viWrite(scope, (ViBuf) cmd   , strlen(cmd)   , &retCount);
    status = viRead( scope, (ViBuf) buffer, sizeof(buffer), &retCount);
    printf("DISPLAY:PERSISTENCE? %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));



    // Query Preamble
    sprintf(cmd, "WFMPRE?\n");
    status = viWrite(scope, (ViBuf) cmd   , strlen(cmd)   , &retCount);
    status = viRead( scope, (ViBuf) buffer, sizeof(buffer), &retCount);
    if(status < VI_SUCCESS) goto error;
    
    printf("WFMPRE?\n   %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));    



    // printf("\n\n");

    return 0;



    error:
        // Report error and clean up
        viStatusDesc(scope, status, buffer);
        fprintf(stderr, "\n       failure: %s\n", buffer);
        return 1;
}
