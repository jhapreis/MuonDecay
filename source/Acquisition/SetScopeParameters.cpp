#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <ctime>

#include <visa.h>

#include "../Configs/cfg.h"
#include "includes/acquisition.h"



int main(int argc, char **argv){



    // Scope parameters
    char err[32];                            // Error string
    ViSession rm          = VI_NULL;         // Resource Manager
    ViSession scope       = VI_NULL;         // Oscilloscope
    ViStatus status_scope = VI_NULL;         // failure or success
    ViUInt32 retCount     = VI_NULL;         // retCount
    ViChar buffer[4*Scope_NumberADChannels]; // Buffer; size > Scope_NumberADChannels



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


    Set_ScopeParameters(status_scope, scope, retCount, argv[1]);


    status_scope = viWrite(scope, (ViBuf) "*IDN?\n"     , 6             , &retCount);
    status_scope = viRead( scope, (ViBuf) buffer        , sizeof(buffer), &retCount);
    if(status_scope < VI_SUCCESS){
        strcpy(err, "Error opening device");
        goto error;
    }
    printf("ID: %s\n\n      ...done\n\n", buffer);



    return 0;



    error:
        // Report error and clean up
        viStatusDesc(scope, status_scope, buffer);
        fprintf(stderr, "\n       %s: failure: %s\n", err, buffer);
        if(rm != VI_NULL) viClose(rm);
        return 1;
}
