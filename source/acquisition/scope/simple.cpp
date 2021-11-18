#include <visa.h>

#include <stdio.h>
#include <memory.h>


int main(int argc, char* argv[]){

    ViSession rm = VI_NULL, vi = VI_NULL;

    ViStatus status;

    ViChar buffer[256];

    ViUInt32 retCnt;



    // Open a default session
    status = viOpenDefaultRM(&rm);
    if(status < VI_SUCCESS){
        goto error;
    }


    // Open USB device
    status = viOpen(rm, "USB0::0x0699::0x0363::04WRL8::INSTR", VI_NULL, VI_NULL, &vi);
    if(status < VI_SUCCESS){
        goto error;
    }


    // Send an ID query
    status = viWrite(vi, (ViBuf) "*idn?", 5, &retCnt);
    if(status < VI_SUCCESS){
        goto error;
    }


    // Clear buffer and read the response
    memset(buffer, 0, sizeof(buffer));
    status = viRead(vi, (ViBuf) buffer, sizeof(buffer), &retCnt);
    if(status < VI_SUCCESS){
        goto error;
    }


    // Print the response
    printf("id: %s\n", buffer);


    // Clean up and end
    viClose(vi);
    viClose(rm);
    return 0;



    error:
        // Report error and clean up
        viStatusDesc(vi, status, buffer);
        fprintf(stderr, "failure: %s\n", buffer);
        if(rm != VI_NULL){
            viClose(rm);
        }
        return 1;
}
