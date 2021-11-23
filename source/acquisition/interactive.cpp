#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include <visa.h>

#include "../configs/cfg.h"
#include "acquisition.h"



int main(){

    char err[32];
    char cmd[32];

    int* WaveformAsInt = NULL;

    double* Waveform_MiliVolts = NULL;

    ViSession rm = VI_NULL, scope = VI_NULL;

    ViStatus status = VI_NULL;

    ViChar buffer[4*Scope_NumberADChannels]; // > Scope_NumberADChannels

    ViUInt32 retCount = VI_NULL;


    // Open a default session and the USB device
    status = viOpenDefaultRM(&rm);
    status = viOpen(rm, USB_Instrument, VI_NULL, VI_NULL, &scope);

    if(status < VI_SUCCESS){
        strcpy(err, "Error opening device");
        goto error;
    }


    // Set the timeout for message-based communication
    status = viSetAttribute(scope, VI_ATTR_TMO_VALUE, Scope_TimeOutValue);

    if(status < VI_SUCCESS){
        strcpy(err, "Error on timeout attribute");
        goto error;
    }


    while(1){

        memset(buffer, 0, sizeof(buffer));

        // Read command
        printf("\nInsert a command: \n");
        scanf("%s", cmd);
        

        if(  strcmp(cmd, "write") == 0  ){ // Write

            scanf("%*c%[^\n]s", cmd);

            status = viWrite(scope, (ViBuf) cmd   , strlen(cmd)   , &retCount);

            if(status < VI_SUCCESS){
                strcpy(err, "Error on query");
                goto error;
            }
        }


        else if(  strcmp(cmd, "read") == 0  ){ // Read
            
            scanf("%*c%[^\n]s", cmd);
            
            status = viRead( scope, (ViBuf) buffer, sizeof(buffer), &retCount);

            if(status < VI_SUCCESS){
                strcpy(err, "Error on query");
                goto error;
            }
        }


        else if(  strcmp(cmd, "query") == 0  ){ // Query method = Write + Read

            scanf("%*c%[^\n]s", cmd);

            status = viWrite(scope, (ViBuf) cmd   , strlen(cmd)   , &retCount);
            status = viRead( scope, (ViBuf) buffer, sizeof(buffer), &retCount);

            if(status < VI_SUCCESS){
                strcpy(err, "Error on query");
                goto error;
            }

            if( strcmp(uppercase(cmd), "CURVE?") == 0 ){
                WaveformAsInt = Get_CurveData(buffer);
                if(WaveformAsInt == NULL) return 1;

                Waveform_MiliVolts   = Convert_WaveformMiliVolts(WaveformAsInt, Scope_NumberADChannels);
                if(Waveform_MiliVolts == NULL) return 1;
                
                for(int i=0; i<Scope_NumberADChannels; i++){
                    printf("%f ", Waveform_MiliVolts[i]);
                }
                printf("\n");
            }
            else{
                printf("\n%s\n", buffer);
            }
        }


        else if(  strcmp(cmd, "set") == 0  ){ // Set Scope Parameters

            Set_ScopeParameters(status, scope, retCount);
        }


        else if(  strcmp(cmd, "f") == 0  ){ // Clean up and end
            
            viClose(scope);
            viClose(rm);
            exit(0);
        }


        else if(  strcmp(cmd, "help") == 0  ){ // Help instructions
            
            printf("You have a few options:\n   query [command]\n   write [command]\n   read [command]\n   set\n   f, to finish\n\n");
        }


        else{

            printf("Invalid command :/\n\n");
        }
    }

    error:
        // Report error and clean up
        viStatusDesc(scope, status, buffer);
        fprintf(stderr, "\n       %s: failure: %s\n", err, buffer);
        if(rm != VI_NULL){
            viClose(rm);
        }
        return 1;
}
