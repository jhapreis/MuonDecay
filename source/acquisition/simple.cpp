// #include ".\visa.h"
// #include ".\vpptype.h"
// #include "TekVisa.h"
// #include "visatype.h"
#include <stdio.h>
#include <memory>
#include <string>

int main (int argc, char* argv[]){

	ViSession rm = VI_NULL, vi = VI_NULL;
	ViStatus status;
	ViChar buffer[256];
	ViUInt32 retCnt;

	status = viOpenDefaultRM(&rm);
	if (status <VI_SUCCESS) goto error;

	status = viOpen(rm, "USB0::0x0699::0x0363::C061073::0::INSTR", VI_NULL, VI_NULL, &vi);

	if (status <VI_SUCCESS) goto error ;

	status = viWrite(vi, (ViBuf) "*idn?", 5, &retCnt);
	if (status <VI_SUCCESS) goto error;

	memset(buffer, 0, sizeof(buffer));
	status = viRead(vi, (ViBuf) buffer, sizeof(buffer), &retCnt);
	if (status<VI_SUCCESS) goto error;

	printf("id: %s\n", buffer);

	viClose(vi);

	viClose(rm);

	return 0;

error:
	viStatusDesc(vi, status, buffer);
	fprintf(stderr, "failure: %s\n",buffer );
	if (rm != VI_NULL){
		viClose(rm);
	}
	return 1;
}
