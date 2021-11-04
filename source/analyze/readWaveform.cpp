#include <TStopwatch.h>

#include "../configs/cfg.h"
#include "analyze.h"



int main(){

    // Measure elapsed time
    TStopwatch time_elapsed;



    printf("\n\n\n");
    printf("//==================================================\n");
    printf("            RUNNING readWaveform.cpp\n");
    printf("//==================================================\n");
    
    Analyze_DataFile("../data", "5555_eventos_T.csv");

    ExponentialFit("../data/5555_eventos_T.root");



    time_elapsed.Print();

    return 0;
}
