#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TH1F.h"
#include <vector>
#include <string>
#include <TH1.h>
#include "TLatex.h"
#include "TLine.h"
#include "TColor.h"
#include "TPaveText.h"
#include <TStyle.h>
#include <TLegend.h>
#include <iostream>
#include <sstream>
using namespace std;

void Print(TCanvas*, std::string);
void histogram_settings(TH1F* &, string, string, string, int);
double GetSeparation( TH1*, TH1*);
TCanvas* Draw_Output(TH1F* &, TH1F* &, std::string , int &);

void training_plots()
{
    
    std::string pairing = "dMMethod";
    std::cout << "Pairing Method : " << pairing << std::endl;

//    std::string bdt_hist_file_name = "ttH_TMVA_dRMethod.root";
    std::string bdt_hist_file_name = "ttH_TMVA_"+pairing+".root";
    
    std::cout << "bdt hist_file_name : " << bdt_hist_file_name << std::endl;
    TFile* HistoSourceFile = new TFile(bdt_hist_file_name.c_str(), "READ");
    TH1F* BDT_signal = (TH1F*)HistoSourceFile->Get("Method_BDT/BDT/MVA_BDT_S");
    TH1F* BDT_bckg = (TH1F*)HistoSourceFile->Get("Method_BDT/BDT/MVA_BDT_B");

//    std::string nn_hist_file_name = "~/Dropbox/ttH_NN/NB_train/ttH_evaluationsPlots_dRMethod.root";
    std::string nn_hist_file_name = "~/Dropbox/ttH_NN/NB_train/ttH_evaluationsPlots_"+pairing+".root";
    TFile* input_nn = new TFile(nn_hist_file_name.c_str(),"READ");

    /*TH1F* NN_signal_odd = (TH1F*)input_nn->Get("ttH_expertise_odd_dRMethod.nb_signal");
    TH1F* NN_signal_even = (TH1F*)input_nn->Get("ttH_expertise_even_dRMethod.nb_signal");
    TH1F* NN_bckg_odd = (TH1F*)input_nn->Get("ttH_expertise_odd_dRMethod.nb_background");
    TH1F* NN_bckg_even = (TH1F*)input_nn->Get("ttH_expertise_even_dRMethod.nb_background");*/
    

    std::cout << "nn_hist_file_name : " << nn_hist_file_name<< std::endl;

    
    std::stringstream nn_sig_odd_ss;
    nn_sig_odd_ss << "ttH_expertise_odd_"<<pairing<<".nb_signal";
    
    std::stringstream nn_sig_even_ss;
    nn_sig_even_ss << "ttH_expertise_even_"<<pairing<<".nb_signal";
    
    std::stringstream nn_bckg_odd_ss;
    nn_bckg_odd_ss << "ttH_expertise_odd_"<<pairing<<".nb_background";
    
    std::stringstream nn_bckg_even_ss;
    nn_bckg_even_ss << "ttH_expertise_even_"<<pairing<<".nb_background";
    
    cout << "nn_sig_odd_ss.str() : " << nn_sig_odd_ss.str() << endl;
    cout << "nn_sig_even_ss.str() : " << nn_sig_even_ss.str() << endl;
    cout << "nn_bckg_odd_ss.str() : " << nn_bckg_odd_ss.str() << endl;
    cout << "nn_bckg_even_ss.str() : " << nn_bckg_even_ss.str() << endl;
    
    TH1F* NN_signal_odd = (TH1F*)input_nn->Get(nn_sig_odd_ss.str().c_str());
    TH1F* NN_signal_even = (TH1F*)input_nn->Get(nn_sig_even_ss.str().c_str());
    TH1F* NN_bckg_odd = (TH1F*)input_nn->Get(nn_bckg_odd_ss.str().c_str());
    TH1F* NN_bckg_even = (TH1F*)input_nn->Get(nn_bckg_even_ss.str().c_str());
    

    NN_signal_odd->Add(NN_signal_even);

    NN_bckg_odd->Add(NN_bckg_even);
    
    int counter=0;

    TCanvas* canvas1 = Draw_Output(BDT_signal, BDT_bckg, pairing, counter);
    std::string output_canvas1_name = "BDT_Output_"+pairing;
    Print(canvas1,output_canvas1_name.c_str());

    counter++;
    
    TCanvas* canvas2 = Draw_Output(NN_signal_odd, NN_bckg_odd, pairing, counter);
    std::string output_canvas2_name = "NN_Output_"+pairing;
    Print(canvas2,output_canvas2_name.c_str());
    
    return;
}

TCanvas* Draw_Output(TH1F* signal_hist, TH1F* bckg_hist, std::string method, int &count){

    cout << "count : " << count << endl;
    std::stringstream cname_ss;
    cname_ss << "c" << count;
    std::stringstream p1name_ss;
    p1name_ss << "p" << count;
    std::stringstream p2name_ss;
    p2name_ss << "p" << count << count;

    char* cname = (cname_ss.str()).c_str();
    char* p1name = (p1name_ss.str()).c_str();
    char* p2name = (p2name_ss.str()).c_str();
    
    TCanvas* canvas = new TCanvas(cname,cname,1000,600);
    canvas->SetBorderMode(0);
    
    double p1Width=0.8;
    TPad * p1 = new TPad(p1name, p1name, 0., 0., p1Width, 1.);
    TPad * p2 = new TPad(p2name, p2name, p1Width, 0.8, 1., 1.);
    
    p1->SetRightMargin(0.05);
    p2->SetLeftMargin(0.0);
    p2->SetRightMargin(0.00);
    p1->Draw();
    p2->Draw();
    p1->cd();
    
    gStyle->SetOptStat(0);
    
    std::string hist_title = "Signal vs. Bckg Discrimination "+method;
    histogram_settings(signal_hist, hist_title, "Disciminator", "Scaled Events", 4);
    signal_hist->Draw();
    histogram_settings(bckg_hist, hist_title, "Disciminator", "Scaled Events", 2);
    bckg_hist->Draw("same");
    
    double separation = GetSeparation(signal_hist,bckg_hist);
    
    std::stringstream sep;
    sep<<"Separation = " << separation;
    
    cout << "separation : " << separation << endl;
    
    std::string sep_str = sep.str();
    TLatex l;
    l.SetNDC();
    l.SetTextFont(22);
    l.SetTextColor(kBlack);
    l.SetTextSize(0.03);
    l.DrawLatex(0.75,0.85,sep_str.c_str());
    
    TLatex p;
    p.SetNDC();
    p.SetTextFont(22);
    p.SetTextColor(kBlack);
    p.SetTextSize(0.03);
    p.DrawLatex(0.15,0.85,"#int L dt = 20.3fb^{-1} 8TeV");
    
    p2->cd();
    TLegend* leg;
    leg = new TLegend(0.,0.,1.,1.);
    leg->SetFillColor(0);
    leg->SetLineColor(0);
    leg->AddEntry(signal_hist, "ttH Signal");
    leg->AddEntry(bckg_hist, "All Background");
    leg->Draw("same");
    
    return canvas;
}

void Print(TCanvas* c1, std::string title){
    
    std::string output_png_name;
    
    output_png_name=title+".png";
    
    c1->Print(output_png_name.c_str(), ".png");
    
    return;
}

void histogram_settings(TH1F* &hist, std::string name, std::string x_title, std::string y_title, int line_colour){
    
    //hist->Sumw2();
    hist->GetXaxis()->SetTitle(x_title.c_str());
    hist->GetXaxis()->SetTitleOffset(1.6);
    hist->GetYaxis()->SetTitle(y_title.c_str());
    hist->GetYaxis()->SetTitleOffset(1.6);
    hist->SetLineColor(line_colour);
    hist->SetTitle(name.c_str());

    
    return;
}
double GetSeparation( TH1* S, TH1* B )
{
    // compute "separation" defined as
    // <s2> = (1/2) Int_-oo..+oo { (S^2(x) - B^2(x))/(S(x) + B(x)) dx }
    Double_t separation = 0;
    
    // sanity checks
    // signal and background histograms must have same number of bins and
    // same limits
    if ((S->GetNbinsX() != B->GetNbinsX()) || (S->GetNbinsX() <= 0)) {
        std::cout << "<GetSeparation> signal and background"
        << " histograms have different number of bins: "
        << S->GetNbinsX() << " : " << B->GetNbinsX() << endl;
        exit(1);
    }
    
    Int_t nstep = S->GetNbinsX();
    Double_t intBin = (S->GetXaxis()->GetXmax() - S->GetXaxis()->GetXmin())/nstep;
    Double_t nS = S->GetSumOfWeights()*intBin;
    Double_t nB = B->GetSumOfWeights()*intBin;
    if (nS > 0 && nB > 0) {
        for (Int_t bin=0; bin<nstep; bin++) {
            Double_t s = S->GetBinContent( bin )/Double_t(nS);
            Double_t b = B->GetBinContent( bin )/Double_t(nB);
            // separation
            if (s + b > 0) separation += 0.5*(s - b)*(s - b)/(s + b);
        }
        separation *= intBin;
    }
    else {
        std::cout << "<GetSeparation> histograms with zero entries: "
        << nS << " : " << nB << " cannot compute separation"
        << endl;
        separation = 0;
    }
    
    return separation;
}