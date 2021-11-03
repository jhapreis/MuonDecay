#include "../configs/cfg.h"
#include "analyze.h"



int main(){

    printf("\n\n\n");
    printf("/==================================================\n");
    printf("            RUNNING readWaveform.cpp\n");
    printf("/==================================================\n");
    
    Analyze_DataFile("../data", "99_eventos_T.csv");
}
