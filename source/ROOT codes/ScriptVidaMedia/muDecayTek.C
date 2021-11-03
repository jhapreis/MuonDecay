/*
Muon Decay Tek
Anderson Fauth. março 2021
*/
#include <fstream>
#include <iostream>
using namespace std;

// exponential + constante function
Double_t f1_AexpX_C(Double_t *x, Double_t *par) {
   //return par[0] + par[1]*x[0] + par[2]*x[0]*x[0];
   return par[0]*exp(-x[0]/par[1])+par[2];
}

Double_t f2_gaus(Double_t *x, Double_t *par){
   return par[0]*exp(-0.5*pow((x[0]-par[1])/par[2],2));
}
Double_t f3_gaus(Double_t *x, Double_t *par){
   return par[0]*exp(-0.5*pow((x[0]-par[1])/par[2],2));
}

// Sum of f1 + f2 + f3
Double_t fitFunction(Double_t *x, Double_t *par) {
  return f1_AexpX_C(x,par);// + f2_gaus(x,&par[3]) + f3_gaus(x,&par[6]);
}



void muDecayTek() {
   int eventos = 0;
   Float_t dTime;

   //create the file, the Tree and branches
   TFile *fileRoot = new TFile("muDecayTek.root","RECREATE");
   TTree *t3 = new TTree("t3","Muon");
   t3->Branch("dTime",&dTime,"dTime/F");

   ifstream infile5;
   string fileData = "5500_diferencias.csv";
   infile5.open(fileData.c_str(), ios::in);
   if(infile5.is_open() && infile5.good())
   {
    printf(" lendo file %s\n ",fileData.c_str());
    while(!infile5.eof()) // leitura arquivo de dados
    {
     infile5 >> dTime;
     if(infile5.eof()) break; //
     t3->Fill();
     eventos++;
    }
    infile5.close(); // fecha arquivo com DADOS brutos
    printf("    Numero eventos = %d \n",eventos);

    fileRoot->Write();

    TCanvas *c2 = new TCanvas("c2","Muon",0,0,800,1000);
    gPad->SetGridx();
    gPad->SetGridy();
    t3->Draw("dTime");
    TH1F *histo = (TH1F*)gPad->GetPrimitive("htemp");

    histo->SetMarkerStyle(21);
    histo->SetMarkerSize(0.8);
    histo->SetLineWidth(4);
    histo->SetStats(11);

    gStyle->SetOptFit(1011);

   // create a TF1 with the range from 0 to 15 and 9 parameters
   TF1 *fitFcn = new TF1("fitFcn",fitFunction,0.1,10.4,3);
   fitFcn->SetNpx(500);
   fitFcn->SetLineWidth(6);
   fitFcn->SetLineColor(kRed);

      // set start values for some parameters
   fitFcn->SetParameter(0,3000);// fitFcn->SetParLimits(0,100,60000);
   fitFcn->SetParameter(1,2.0); //fitFcn->SetParLimits(1,1.1,3.0);
   fitFcn->SetParameter(2,300);// fitFcn->SetParLimits(2,200,400);


    histo->Fit("fitFcn","R");

    //htemp->GetXaxis()->SetTitle("Time (seconds)");
    // Yaxis
    //htemp->GetYaxis()->SetTitle("Tank Rate (Hz)");

    c2->Modified();
    c2->Update();
   }
}



