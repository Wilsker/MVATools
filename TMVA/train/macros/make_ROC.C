#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TDirectory.h"
#include "TH1F.h"
#include "TH2F.h"
#include <vector>
#include <string>
#include "TH1.h"
#include "TLatex.h"
#include "TLine.h"
#include "TColor.h"
#include "TPaveText.h"
#include <TStyle.h>
#include <TLegend.h>
#include <iostream>
#include <sstream>
#include "TF1.h"
#include "TSystem.h"
#include "TMath.h"

#include "TGraph.h"
using namespace std;

void Print(TCanvas*, std::string);

void histogram_settings(TH2F* &, string, string, string, int);

void make_data(std::vector<double> &x_values, std::vector<double> &y_values, TH1F*, TH1F*);

void make_data(std::vector<double> &x_values, std::vector<double> &y_values, TH1F* NN_sig_odd, TH1F* NN_bg_odd, TH1F* NN_sig_even, TH1F* NN_bg_even);

void make_ROC(){
    
    std::string bdt_hist_file_name = "ttH_TMVA_dRMethod.root";
    std::string dM_bdt_hist_file_name = "ttH_TMVA_dMMethod.root";

    TFile* input_tmva = new TFile(bdt_hist_file_name.c_str(),"READ");
    TFile* dM_input_tmva = new TFile(dM_bdt_hist_file_name.c_str(),"READ");

    std::string nn_hist_file_name = "~/Dropbox/ttH_NN/NB_train/ttH_evaluationsPlots_dRMethod.root";
    TFile* input_nn = new TFile(nn_hist_file_name.c_str(),"READ");
    
    TH1F* BDT_signal = (TH1F*)input_tmva->Get("Method_BDT/BDT/MVA_BDT_S");
    TH1F* BDT_bckg = (TH1F*)input_tmva->Get("Method_BDT/BDT/MVA_BDT_B");

    TH1F* dM_BDT_signal = (TH1F*)dM_input_tmva->Get("Method_BDT/BDT/MVA_BDT_S");
    TH1F* dM_BDT_bckg = (TH1F*)dM_input_tmva->Get("Method_BDT/BDT/MVA_BDT_B");

    TH1F* NN_signal_odd = (TH1F*)input_nn->Get("ttH_expertise_odd_dRMethod.nb_signal");
    TH1F* NN_signal_even = (TH1F*)input_nn->Get("ttH_expertise_even_dRMethod.nb_signal");
    TH1F* NN_bckg_odd = (TH1F*)input_nn->Get("ttH_expertise_odd_dRMethod.nb_background");
    TH1F* NN_bckg_even = (TH1F*)input_nn->Get("ttH_expertise_even_dRMethod.nb_background");
    
   
    cout << "=============== BDT ===============" << endl;
    std::vector<double> x_bdt;
    std::vector<double> y_bdt;
    make_data(x_bdt,y_bdt,BDT_signal,BDT_bckg);
    
    //####### dM BDT #######
    std::vector<double> dM_x_bdt;
    std::vector<double> dM_y_bdt;
    make_data(dM_x_bdt,dM_y_bdt,dM_BDT_signal,dM_BDT_bckg);
    
    cout << "=================== NN ===================" << endl;
    std::vector<double> x_NN;
    std::vector<double> y_NN;
    make_data(x_NN ,y_NN ,NN_signal_odd, NN_bckg_odd, NN_signal_even, NN_bckg_even);

    TCanvas* canvas = new TCanvas("c1","c1",1000,600);
    canvas->SetBorderMode(0);

    double p1Width=0.8;
    double p2height=0.8;
    TPad * p1 = new TPad("p1", "p1", 0., 0., p1Width, 1.);
    TPad * p2 = new TPad("p2", "p2", p1Width, 0.8, 1., 1.);
    TPad * p3 = new TPad("p2", "p2", p1Width, 0., 1., p2height);
    
    p1->SetRightMargin(0.05);
    p2->SetLeftMargin(0.0);
    p2->SetRightMargin(0.00);
    p1->SetGrid();
    p1->Update();
    p1->Draw();
    p2->Draw();
    p3->Draw();
    p1->cd();
    gStyle->SetOptStat(0);
    
    int bdt_color = 4;
    int dM_bdt_color = 2;
    int nn_color = 1;

    TGraph* roc_bdt_graph = new TGraph(x_bdt.size(),&x_bdt[0], &y_bdt[0]);
    TGraph* dM_roc_bdt_graph = new TGraph(dM_x_bdt.size(),&dM_x_bdt[0], &dM_y_bdt[0]);
    TGraph* roc_nn_graph = new TGraph(x_NN.size(), &x_NN[0], &y_NN[0]);
    
    std::string bdt_name = "Signal Eff. Vs. Bckg Rej. for MVA Classifiers";
    std::string x_title = "Signal Efficiency";
    std::string y_title = "Bckg Rejection";
    roc_bdt_graph->GetXaxis()->SetTitle("Signal Efficiency");
    roc_bdt_graph->GetYaxis()->SetTitle("Bckg Rejection");
    roc_bdt_graph->SetLineColor(bdt_color);
    roc_bdt_graph->SetTitle(bdt_name.c_str());
    roc_bdt_graph->Draw("Al");
    
    dM_roc_bdt_graph->GetXaxis()->SetTitle("Signal Efficiency");
    dM_roc_bdt_graph->GetYaxis()->SetTitle("Bckg Rejection");
    dM_roc_bdt_graph->SetLineColor(dM_bdt_color);
    dM_roc_bdt_graph->SetTitle(bdt_name.c_str());
    dM_roc_bdt_graph->Draw("same");
    
    roc_nn_graph->GetXaxis()->SetTitle("Signal Efficiency");
    roc_nn_graph->GetYaxis()->SetTitle("Bckg Rejection");
    roc_nn_graph->SetLineColor(nn_color);
    roc_nn_graph->SetTitle(bdt_name.c_str());
    roc_nn_graph->Draw("same");
    
    p2->cd();
    cout << "Printing Legend . . . " << endl;
    
    TLegend* leg;
    leg = new TLegend(0.1,0.1,0.9,0.8);
    leg->SetFillColor(0);
    leg->SetLineColor(0);
    leg->AddEntry(roc_bdt_graph, "dR Method BDT","l");
    leg->AddEntry(dM_roc_bdt_graph, "dM Method BDT", "l");
    leg->AddEntry(roc_nn_graph, "dR Method NN", "l");
    leg->Draw("histsame");

    p3->cd();
    cout << "Printing Fit Information . . . . " << endl;
    
    string output_canvas_name = "ROC_Curves";
    Print(canvas,output_canvas_name.c_str());
    
    delete roc_bdt_graph;
    roc_bdt_graph = NULL;
    
    delete roc_nn_graph;
    roc_nn_graph = NULL;
    
    delete input_tmva;
    input_tmva = NULL;
    
    
    delete input_nn;
    delete p1;
    delete p2;
    delete leg;
    
    input_nn = NULL;
    p1 = NULL;
    p2 = NULL;
    leg = NULL;
    
    canvas->Clear();
    canvas->Closed();
    canvas = NULL;
    return;
}

void make_data(std::vector<double> &x_values, std::vector<double> &y_values, TH1F* BDT_sig, TH1F* BDT_bg){
    
    vector<double> bdt_cuts;
    bdt_cuts.clear();
    x_values.clear();
    y_values.clear();

    double bdt_Nbins = BDT_bg->GetXaxis()->GetNbins();
    double cut_value_bdt;
    
    for (int k =0; k<bdt_Nbins; k++) {
        cut_value_bdt = BDT_bg->GetXaxis()->GetBinLowEdge(k);
        bdt_cuts.push_back(cut_value_bdt);
    }
    
    for (int j =0; j<bdt_cuts.size(); j++) {
        
        double bdt_n_signal_pass_cut=0;
        double bdt_n_signal_total=0;
        double bdt_n_bckg_pass_cut=0;
        double bdt_n_bckg_total=0;
        float bdt_signal_eff=0;
        float bdt_bckg_rej=0;
        
        double temp_classifier_cut = bdt_cuts.at(j) ;
        
        for (int i=0; i<BDT_sig->GetNbinsX(); i++) {
            double bdt_classifier_value = BDT_sig->GetXaxis()->GetBinLowEdge(i);
            double temp_bin_content = BDT_sig->GetBinContent(i);
            bdt_n_signal_total=bdt_n_signal_total+temp_bin_content;
            bdt_n_signal_pass_cut = (bdt_classifier_value >= temp_classifier_cut ? bdt_n_signal_pass_cut+temp_bin_content : bdt_n_signal_pass_cut);
        }
        
        for (int i=0; i<BDT_bg->GetNbinsX(); i++) {
            double bdt_classifier_value = BDT_bg->GetXaxis()->GetBinLowEdge(i);
            double temp_bin_content = BDT_bg->GetBinContent(i);
            bdt_n_bckg_total = bdt_n_bckg_total+temp_bin_content;
            bdt_n_bckg_pass_cut = (bdt_classifier_value >= temp_classifier_cut ? bdt_n_bckg_pass_cut+temp_bin_content : bdt_n_bckg_pass_cut);
        }
        
        bdt_signal_eff=bdt_n_signal_pass_cut/bdt_n_signal_total ;
        bdt_bckg_rej= 1-(bdt_n_bckg_pass_cut/bdt_n_bckg_total);
        if (bdt_n_signal_total>0 && bdt_n_bckg_total>0) {
            x_values.push_back(bdt_signal_eff);
            y_values.push_back(bdt_bckg_rej);
        }
    }
    return;
}

void make_data(std::vector<double> &x_values, std::vector<double> &y_values, TH1F* NN_sig_odd, TH1F* NN_bg_odd, TH1F* NN_sig_even, TH1F* NN_bg_even){
    
    double cut_value_nn;
    vector<double> nn_cuts;
    double nn_Nbins = NN_bg_odd->GetXaxis()->GetNbins();
    
    for (int k =0; k<nn_Nbins; k++) {
        cut_value_nn = NN_bg_odd->GetXaxis()->GetBinLowEdge(k);
        nn_cuts.push_back(cut_value_nn);
    }
    
    for (int j =0; j<nn_cuts.size(); j++) {
        double nn_n_signal_pass_cut=0;
        double nn_n_signal_total=0;
        double nn_n_bckg_pass_cut=0;
        double nn_n_bckg_total=0;
        float nn_signal_eff=0;
        float nn_bckg_rej=0;
        
        double temp_classifier_cut = nn_cuts.at(j) ;
        for (int i=0; i<NN_sig_odd->GetNbinsX(); i++) {
            double nn_classifier_value = NN_sig_odd->GetXaxis()->GetBinLowEdge(i);
            double temp_bin_content = NN_sig_odd->GetBinContent(i);
            nn_n_signal_total=nn_n_signal_total+temp_bin_content;
            nn_n_signal_pass_cut = (nn_classifier_value >= temp_classifier_cut ? nn_n_signal_pass_cut+temp_bin_content : nn_n_signal_pass_cut);
        }
        for (int i=0; i<NN_sig_even->GetNbinsX(); i++) {
            double nn_classifier_value = NN_sig_even->GetXaxis()->GetBinLowEdge(i);
            double temp_bin_content = NN_sig_even->GetBinContent(i);
            nn_n_signal_total=nn_n_signal_total+temp_bin_content;
            nn_n_signal_pass_cut = (nn_classifier_value >= temp_classifier_cut ? nn_n_signal_pass_cut+temp_bin_content : nn_n_signal_pass_cut);
        }
        
        for (int i=0; i<NN_bg_odd->GetNbinsX(); i++) {
            double nn_classifier_value = NN_bg_odd->GetXaxis()->GetBinLowEdge(i);
            double temp_bin_content = NN_bg_odd->GetBinContent(i);
            nn_n_bckg_total = nn_n_bckg_total+temp_bin_content;
            nn_n_bckg_pass_cut = (nn_classifier_value > temp_classifier_cut ? nn_n_bckg_pass_cut+temp_bin_content : nn_n_bckg_pass_cut);
        }
        for (int i=0; i<NN_bg_even->GetNbinsX(); i++) {
            double nn_classifier_value = NN_bg_even->GetXaxis()->GetBinLowEdge(i);
            double temp_bin_content = NN_bg_even->GetBinContent(i);
            nn_n_bckg_total = nn_n_bckg_total+temp_bin_content;
            nn_n_bckg_pass_cut = (nn_classifier_value > temp_classifier_cut ? nn_n_bckg_pass_cut+temp_bin_content : nn_n_bckg_pass_cut);
        }
        
        nn_signal_eff=nn_n_signal_pass_cut/nn_n_signal_total ;
        nn_bckg_rej= 1-(nn_n_bckg_pass_cut/nn_n_bckg_total);
        
        if (nn_n_signal_pass_cut>0 && nn_n_bckg_pass_cut>0) {
            x_values.push_back(nn_signal_eff);
            y_values.push_back(nn_bckg_rej);
        }
    }
    return;
}

void Print(TCanvas* c1, std::string title){
    
    std::string output_png_name;
    
    output_png_name= title+".png";
    
    c1->Print(output_png_name.c_str(), ".png");
    
    return;
}

void histogram_settings(TH2F* &hist, std::string name, std::string x_title, std::string y_title, int line_colour){
    
    hist->Sumw2();
    hist->SetMarkerSize(5);
    hist->SetMarkerStyle(2);
    hist->SetMarkerColor(line_colour);
    hist->GetXaxis()->SetTitle(x_title.c_str());
    hist->GetXaxis()->SetTitleOffset(1.6);
    hist->GetYaxis()->SetTitle(y_title.c_str());
    hist->GetYaxis()->SetTitleOffset(1.6);
    hist->SetLineColor(line_colour);
    hist->SetTitle(name.c_str());
    
    
    return;
}