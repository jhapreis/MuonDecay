#include <TStopwatch.h>

#include "../acquisition/cfg.h"
#include "analyze.h"



int main(int argc, char **argv){

    // Measure elapsed time
    TStopwatch time_elapsed;



    printf("\n\n\n");
    printf("//==================================================\n");
    printf("            RUNNING readWaveform.cpp\n");
    printf("//==================================================\n");
    
    // Analyze_ROOTDataFile("../data/results/10_1638138645.root");
    for(int i=1; i<argc; i++){
        Analyze_ROOTDataFile(argv[i]);
        ExponentialFit(argv[i]);
    }


    time_elapsed.Print();

    return 0;
}
