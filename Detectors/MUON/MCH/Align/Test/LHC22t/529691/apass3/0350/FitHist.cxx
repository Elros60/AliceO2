#include "TCanvas.h"
#include "TAxis.h"
#include "TH1.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TF1.h"
#include "Math/PdfFuncMathCore.h"


double background(double *x, double *par){
	return par[0] + par[1]*x[0] + par[2]*x[0]*x[0];


}

double lorentzian(double *x, double *par){
	//return (0.5*par[0]*par[1]/TMath::Pi()) / TMath::Max(1.e-10,
    //  (x[0]-par[2])*(x[0]-par[2])+ .25*par[1]*par[1]);

    return ROOT::Math::cauchy_pdf(x[0],par[0],par[1]);
}

double fitFunction(double *x, double *par){
	return background(x,par) + lorentzian(x,&par[3]);
}



void FitHist(){

	TFile *f = TFile::Open("Residual.root");
	TTree *t = (TTree*)f->Get("TreeE");

	TH1F *hist = new TH1F("residual","residual",500,-0.3,0.3);
	t->Draw("fClusterY-fTrackY>>hist","fClDetElem==825 && abs(fClusterY-fTrackY)<0.3","goff");
	TF1 *fitFcn = new TF1("fitFcn",fitFunction,-0.3,0.3,6);
	
	hist->Fit("fitFcn");



}