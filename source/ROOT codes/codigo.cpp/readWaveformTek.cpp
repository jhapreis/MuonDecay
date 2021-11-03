/*
Anderson Fauth.   março 2021
g++ nome.cpp -o nome.exe `root-config --cflags --glibs`
Utilisado ROOT6.
No desenvolvimento desse código foi utilizado o CodeBlocks com:
Settings Compiler->Selected Compiler: GNU GCC Compiler
                 ->Compiler Flags: C++11 ISO
                 ->Linker Settings:
                 ->Other Options: `root-config --cflags --glibs`
                 ->Search Diretories->Compiler: /home/fauth/Desktop/root_v622/root/include
                                    ->Linker: /usr/lib
IMPORTANTE: para carregar as variáveis do ROOT6 no CodeBlocks abra um terminal, carregue as variáveis do ROOT,
e então abra o CodeBlocks com um comendo de linha (codeblocks).

Este código com C/C++ lê os dados do decaimento de múons obtidos com códigos Phyton
e oscilóscopio Tektronix utilizando. O arquivo de dados brutos utilizado são as waveforms que
contêm dois pulsos numa janela de 10microsegundos. As waveforms possuem 2500 pontos e são obtidas
com a uma taxa de 1/250MHz.
Após a leitura dos dados são criadas duas TTree do ROOT-CERN, uma para o display da waveform e outra
com os valores representativos de cada waveform, como: carga e pico dos dois pulsos e diferença de tempo
entre os dois pulsos registrados nas waveforms.
OBS: no futuro pode ser implentado o tempo em UTC, segundos após Epoch, para cada evento.
O arquivo de saída é um arquivo .root e com ele é possível fazer todas as análises necessárias, como:
histograma das diferenças de tempo, ajustes de modelos e obtenção da vida média do múon,
espectros de carga do primeiro e do segundo pulso, filtros utilizando valores de carga, pico, diferença de tempo.
O espectro de Michel obtido pode (ainda não fizemos) ser comparado com a previsão teórica.

Pré-requisitos:
 - arquivo contendo os endereços/nomes dos arquivos contendos os dados brutos
    - pode ficar no mesmo diretório do executável ou ser um argument
    >readWaveformTek.exe [fileTek.tex]
    obs: não testado com vários arquivos, pois no momento temos somente um arquivo de dados.
 Saídas:
  - tres arquivos
    - name.root->com as duas TTrees (t0 e t1)
    - name.log ->com informaçoes dos arquivos (atualmente pouco usado)
    - name.png ->com gráfico dos 100 primeiros pulsos

*/

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "Riostream.h"
#include <time.h> // time_t, struct tm, difftime, time, mktime
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>

#include "TROOT.h"
#include "TLatex.h"
#include <TMinuit.h>
#include <TStyle.h>
#include <TFile.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TCanvas.h>
#include <TPaveText.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>
#include <TTree.h>
using namespace std;

#define NCH 1   //adc channels
#define NWF 2500  //waveform length
#define adcFreqNominal 250000000 // acf: 250MHz? confirmar lendo o manual do osciloscópio

int main(int argc, char *argv[])
{
 // o fileName contendo os arquivos a serem analisados
 // pode ser lido digitando o seu enderiço/nome ou
 // uando filesAnalise.txt no local do executavel
 char FileName[100];
 if (argc > 1) strcpy(FileName, argv[1]);
 else strcpy(FileName, "../fileTek.txt");
  //baseline. Inicialmente foi medida, se mudar ch ou osciloscpio deve ser revista
 float base = {52.38};
 float adcFull=255.; // acf 8bits, 0-255 nao sei se esta correto
 float maxVolt=0.100; // em Volts, acf  pode variar
 float R = 50.;// resistence (Ohm)

 char nameD[100]; //parte do file data com nome detector e data
 char name2[100];

 //variáveir para futura implementação de taxas e tempos dos eventos
 //int dT=60; // interval (in seconds) to calculate the count rate (Hz)
 //int Nt0=100; // numero de waveforms to fill t0 TTree = 1/Nt0
 //int timeE; // Epoch time
 //int timeS; // seconds from time0, first time in the data file
 //int time0=0; // first Epoch time of the data file
 //int time1=0; // Epoch time to calculate the pulse rate, rate=cpul/(timeE-time1)
 //int cpul=0; // pulse counter to calculate the pulse rate
 //int npul = -1; //
 //float rate; // pulse rate (Hz)
 //struct tm  ts;
 //char       buf[80];

 int ID=0; // pulse identifier
 float VoltCh=maxVolt/adcFull;
 int TotGeral = 0;
 int pulsesF = 0; // pulses in one file
 float binTime; // time interval between ADC channels
 binTime = (1.0E09/adcFreqNominal); // ns

 double dtime; //delta time (ns) between two pulses
 int ch; // ADC values
 float amp; //amplitude em mV
 float timePul; //time pulse (ns)
 double charge1,charge2;// pulse charge (pC)
 double peak1,peak2; //  pulse amplitud, peak (mV)
 int ip1,ip2; //posição dos picos
 float sum1,sum2; // sum of the ADC channels 1, 2 and 3

 //...open file containing the file names to be analyzed
 string fileAnalise = FileName;
 ifstream infile0;
 infile0.open(fileAnalise.c_str(),ios::in);
 if(infile0.is_open() && infile0.good())
 {printf ("opening %s\n ",fileAnalise.c_str());}
  else {printf ("error opening! %s\n ",fileAnalise.c_str()); return (1);}

 string file;
 infile0 >> file;//read the name of the first data file

 // search from beginning of string
 std::string::size_type ni,nf;
 ni = file.rfind("/"); //posição do /
 nf = file.find(".csv");//posição do .csv
 //nameD é retirado do nome do file de dados
 strcpy(nameD,file.substr(ni+1,nf-ni-1).c_str());

 strcpy(name2,nameD);
 strcat(name2,".log");
 //string fileLog;
 ofstream fileLog;
 fileLog.open(name2,ios::out);
 if(fileLog.is_open() && fileLog.good())
 {printf ("opening %s\n ",name2);}
  else {printf ("error opening! %s\n ",name2); return (1);}

 strcpy(name2,nameD);
 strcat(name2,".root");
 TFile *fileRoot = new TFile(name2,"RECREATE");

 strcpy(name2,nameD);
 strcat(name2, "Waveform");
 TTree *t0 = new TTree("t0",name2);
 t0->Branch("ID",&ID,"ID/I");
 t0->Branch("amp",&amp,"amp/F");
 t0->Branch("timePul",&timePul,"timePul/F");

 strcpy(name2,nameD);
 strcat(name2, "ChargePeak");
 TTree *t1 = new TTree("t1",name2);
 t1->Branch("ID",&ID,"ID/I");
 t1->Branch("dtime",&dtime,"dtime/D"); //dtime entre dois pulsos
 t1->Branch("charge1",&charge1,"charge1/D"); //
 t1->Branch("charge2",&charge2,"charge2/D");
 t1->Branch("peak1",&peak1,"peak1/D");
 t1->Branch("peak2",&peak2,"peak2/D");

 while(!infile0.fail()) //reading the names of the files
 {

  ifstream infile1; // data files
  infile1.open(file.c_str(), ios::in);
  if(infile1.is_open() && infile1.good())
  {
   printf(" reading %s\n",file.c_str());
   pulsesF = 0;
   string line;
   //5.600000000000000000e+01,
    float wave[NWF];
    string myString;
    while(std::getline(infile1,line)) //loop reading all file data lines
    {
     sum1 = 0; sum2=0;
     peak1 = 0; peak2=0;ip2=0;
     stringstream ss(line);

     for(int i=0;i<NWF;i++)
     {
      getline(ss, myString,',');
      wave[i]=stof(myString);

      if (i>110){ //encontro a posição do 2o. pico
       if (wave[i] < peak2){// pulso NEGATIVO
         peak2 = wave[i];
         ip2=i;
       }

      }

      ch=wave[i];
      amp = (ch-base)*VoltCh; //mV
      timePul=i*1.E09/adcFreqNominal;//ns
      t0->Fill(); //waveform
     }
     //cout << ip2 << " " << peak2 << endl;

     for(int i=90;i<111;i++){// 1o. pico é fixo nesta janela
      sum1=sum1+wave[i];     //se mudar o trigger estes valores mudam!
      if (wave[i] < peak1){
       peak1 = wave[i];//pulso NEGATIVO
       ip1=i;
      }
     }

     //integro entre -5ch pico +15ch
     int ipM;
     if(((ip2+15)>2499)){ipM=2499;}
     else{ ipM=ip2+15;}

     for(int i=(ip2-5);i<ipM;i++){// 2o. pico
      sum2=sum2+wave[i];
     }

     peak1=(peak1-base)*VoltCh; // pulse peak (mV)
     charge1=(sum1-float(NWF)*base)*binTime*VoltCh/R; // pulse charge(pC)
     peak2=(peak2-base)*VoltCh; // pulse peak (mV)
     charge2=(sum2-float(NWF)*base)*binTime*VoltCh/R; // pulse charge(pC)

     if(charge1>=0 || charge2>=0){
      cout << "ID=" ID << " Q1=" << charge1 << " Q2=" << charge2 << endl;
     }

     dtime = (ip2 - ip1)*binTime;
     t1->Fill();
     ID++;
     pulsesF++;
    }

  }else {printf ("Error opening! %s\n ",file.c_str());}

  TotGeral=TotGeral+pulsesF;
  printf("pulsesF = %d  \n",pulsesF);
  fileLog << "pulsesF = " << pulsesF << endl;

  infile0 >> file; // read name next data file
 }

 printf("\n*********************************\n");
 printf("Total pulsesF = %d \n",TotGeral);
 fileRoot->Write();
//********************************************

 TCanvas *c1 = new TCanvas("c1",nameD,0,0,900,1000);
 gPad->SetGridx();
 gPad->SetGridy();
 t0->Draw("amp:timePul","ID<100","l");
 strcpy(name2,nameD);
 strcat(name2, ".png");
 c1->SaveAs(name2);
}
