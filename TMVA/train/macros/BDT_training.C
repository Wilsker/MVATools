#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <TFile.h>
#include <TString.h>
#include <TMVA/Factory.h>

int main(){
    std::string nVars = "nVars_5";

    std::string signal_filename = "/publicfs/cms/data/TopQuark/ttHbb/JTW/2017_07_v1/ttHACIHEP/output/MC/ttHbb/ttHbb_Merged_rootplas.root";
    std::string bckg_filename = "/publicfs/cms/data/TopQuark/ttHbb/JTW/2017_07_v1/ttHACIHEP/output/MC/ttjets_incl/ttjets_incl_Merged_rootplas.root";

    //std::string signal_filename = "~/Dropbox/ttH_NN/NB_train/data/TRF_analysis/ttH_pythia_combined_4_4_DL_w_TRF_"+nVars+"_full_sel.root";
    //std::string bckg_filename = "~/Dropbox/ttH_NN/NB_train/data/TRF_analysis/bckg_MC_combined_4_4_DL_w_TRF_"+nVars+"_full_sel.root";
    TFile* input_file_signal = new TFile(signal_filename.c_str());
    TFile* input_file_bckg = new TFile(bckg_filename.c_str());

    std::string outfile_name_even = "ttHbb_TMVABDT_"+nVars+"_EVEN.root";
    std::string outfile_name_odd = "ttHbb_TMVABDT_"+nVars+"_ODD.root";

    TFile* output_file_EVEN = TFile::Open(outfile_name_even.c_str(),"RECREATE");
    TFile* output_file_ODD = TFile::Open(outfile_name_odd.c_str(),"RECREATE");

    TMVA::Factory* factory_even = new TMVA::Factory("tmvaFactory_even",output_file_EVEN,"");
    TMVA::Factory* factory_odd = new TMVA::Factory("tmvaFactory_odd",output_file_ODD,"");

    std::cout<< "Get Signal tree : " << signal_filename << std::endl;
    TTree* signal = (TTree*)input_file_signal->Get("BOOM");
    std::cout<< "Get Background tree : " << bckg_filename << std::endl;
    TTree* bckg = (TTree*)input_file_bckg->Get("BOOM");

    std::cout<< "Get signal entries " << std::endl;
    int signal_entries = signal->GetEntries();
    std::cout<< "# signal_entries : " << signal_entries << std::endl;

    std::cout<< "Get background entries " << std::endl;
    int bckg_entries = bckg->GetEntries();
    std::cout<< "# bckg_entries : " << bckg_entries << std::endl;

    double signal_weights=1.0;
    double bckg_weights=1.0;

    TBranch *b_signal_weights;
    signal->SetBranchAddress("bWeight", &signal_weights, &b_signal_weights);
    TBranch *b_bckg_weights;
    bckg->SetBranchAddress("bWeight", &bckg_weights, &b_bckg_weights);

    int eventNumber_sig;
    int eventNumber_bckg;

    TBranch *b_eventNumber_sig;
    signal->SetBranchAddress("EVENT_event", &eventNumber_sig, &b_eventNumber_sig);
    TBranch *b_eventNumber_bckg;
    bckg->SetBranchAddress("EVENT_event", &eventNumber_bckg, &b_eventNumber_bckg);

    TCut cutEven = "(Entry$%2==0)";
    TCut cutOdd = "(Entry$%2==1)";

    std::cout << "Add Tree Signal Even Train" << std::endl;
    factory_even->AddTree(signal, "Signal", 1, cutEven, "train");

    std::cout << "Add Tree Bckg Even Train" << std::endl;
    factory_even->AddTree(bckg, "Background", 1, cutEven, "train");

    std::cout << "Add Tree Signal Odd Test" << std::endl;
    factory_even->AddTree(signal, "Signal", 1, cutOdd, "test");

    std::cout << "Add Tree Bckg Odd Test" << std::endl;
    factory_even->AddTree(bckg, "Background", 1, cutOdd, "test");

    std::cout << "Add Tree Signal Odd Train" << std::endl;
    factory_odd->AddTree(signal, "Signal", signal_weights, cutOdd, "train");

    std::cout << "Add Tree Bckg Odd Train" << std::endl;
    factory_odd->AddTree(bckg, "Background", bckg_weights, cutOdd, "train");

    std::cout << "Add Tree Signal Even Test" << std::endl;
    factory_odd->AddTree(signal, "Signal", signal_weights, cutEven, "test");

    std::cout << "Add Tree Bckg Even Test" << std::endl;
    factory_odd->AddTree(bckg, "Background", bckg_weights, cutEven, "test");

    std::cout << "Add variables" << std::endl;

    factory_even->AddVariable("allSumPt_wMET := all_sum_pt_with_met" , 'F');
    factory_even->AddVariable("invMassAll := invariant_mas_of_everything" , 'F');

    factory_odd->AddVariable("allSumPt_wMET := all_sum_pt_with_met" , 'F');
    factory_odd->AddVariable("invMassAll := invariant_mas_of_everything" , 'F');


    TCut myCut="";

    std::cout << "Preparing training and testing ttree . . . . . " << std::endl;
    //Arguments: cut, NSigTrain, NBckgTrain, NSigTest, NBckgTest

    factory_even->PrepareTrainingAndTestTree(myCut, signal_entries/2, bckg_entries/2, signal_entries/2, bckg_entries/2);
    factory_odd->PrepareTrainingAndTestTree(myCut, signal_entries/2, bckg_entries/2, signal_entries/2, bckg_entries/2);

    std::cout << "Booking method . . . . . " << std::endl;
    factory_even->BookMethod(TMVA::Types::kBDT, "BDT", "H:V:NTrees=850:MaxDepth=6:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex");
    factory_odd->BookMethod(TMVA::Types::kBDT, "BDT", "H:V:NTrees=850:MaxDepth=6:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex");

    std::cout << "Training Method . . . " << std::endl;
    factory_even->TrainAllMethods();
    factory_odd->TrainAllMethods();

    std::cout << "Testing Method  . . . " << std::endl;
    factory_even->TestAllMethods();
    factory_odd->TestAllMethods();

    std::cout << "Evaluating Method . . . . " << std::endl;
    factory_even->EvaluateAllMethods();
    factory_odd->EvaluateAllMethods();

    std::cout << "Even factory object written to file : " << output_file_EVEN->GetName() << std::endl;
    std::cout << "Odd factory object written to file : " << output_file_ODD->GetName() << std::endl;
    output_file_EVEN->Close();
    output_file_ODD->Close();
    std::cout << "FIN." << std::endl;

    //delete factory;
    delete factory_even;
    delete factory_odd;
    return 0;

}
