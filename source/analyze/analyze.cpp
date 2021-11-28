#include <TStopwatch.h>

#include "../configs/cfg.h"
#include "../acquisition/peaks.h"
#include "analyze.h"



int main(){

    // Measure elapsed time
    TStopwatch time_elapsed;



    printf("\n\n\n");
    printf("//==================================================\n");
    printf("            RUNNING readWaveform.cpp\n");
    printf("//==================================================\n");
    
    Analyze_ROOTDataFile("../data/results/1000_1638087362_copy.root");

    printf("ascsfafdasdf");


    time_elapsed.Print();

    return 0;
}
