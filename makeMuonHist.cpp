/*
"""
2023/07/27
Arka Santra

Script that makes the hisotgrams for data or MC from ntuples.


To run the script:

string:: root file to process
string:: the output root file name

to run:
go inside root
then for data:

.L makeMuonHist.cpp++
makeMuonHist()

For MC, it is advisable to use the WIS grid
"""
*/

#include <iostream>
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TString.h"
#include "TCollection.h"
#include "TLorentzVector.h"
#include "TKey.h"
#include "TClass.h"
#include "TMath.h"
#include <string>
#include <sys/stat.h>
#include "TChain.h"
#include "TTree.h"
#include <ROOT/RDataFrame.hxx>
#include <algorithm>
#include <time.h>
#include <chrono> // for high_resolution_clock
#include <sstream>
#include <cmath>
#include <ROOT/RDF/HistoModels.hxx>
#include "inputFunctions.h"

using namespace std;
using namespace ROOT;
bool debug = false;

double returnWeight(float wgt, string sample){
    if (sample=="Data")
        return 1.0;
    else
        return wgt;
}

/// prepare 1D histograms from RDataFrame
void prepare1DHistogram(ROOT::RDF::RInterface<ROOT::Detail::RDF::RJittedFilter, void> dtmp, string suffixname, map<string, ROOT::RDF::TH1DModel> allHisto1Dict, map<string, TH1D*> &out1DHistoDict){
    int counter = 0;
    for (map<string, ROOT::RDF::TH1DModel>::iterator it = allHisto1Dict.begin(); it != allHisto1Dict.end(); ++it)
    {
        counter++;
        auto hist = dtmp.Histo1D(it->second, it->first, "weight");
        stringstream ss2;
        ss2 << it->first;
        TH1D *histos = (TH1D*)hist->Clone((ss2.str()+"_"+suffixname).c_str());
        histos        = getOverflow(histos);
        histos        = getUnderflow(histos);
        out1DHistoDict.insert(make_pair(ss2.str()+"_"+suffixname, histos));
    }
    cout << "prepared " << counter << " 1D histograms for " << suffixname << " working point." << endl;
}
    
/// prepare 2D histograms from RDataFrame
void prepare2DHistogram(ROOT::RDF::RInterface<ROOT::Detail::RDF::RJittedFilter, void> dtmp, string suffixname, map<string, manyMaps> allHisto2Dict, map<string, TH2D*> &out2DHistoDict){
    int counter2 = 0;
    for (map<string, manyMaps>::iterator it = allHisto2Dict.begin(); it != allHisto2Dict.end(); ++it)
    {
        counter2++;
        auto hist = dtmp.Histo2D(it->second.outTH2DModel(it->first), it->second.outFirst(it->first), it->second.outSecond(it->first), "weight");
        stringstream ss2;
        ss2 << it->first;
        TH2D *histos = (TH2D*)hist->Clone((ss2.str()+"_"+suffixname).c_str());
        out2DHistoDict.insert(make_pair(ss2.str()+"_"+suffixname, histos));
    }
    cout << "prepared " << counter2 << " 2D histograms for " << suffixname << " working point." << endl;
}


void makeMuonHist(string inputFolder="user.asantra.data23_13p6TeV.00456409.physics_Main.merge.AOD.f1369_m2191.MCP_TESTNTUP_ANALYSIS.root/", string outputFile="data23_13p6TeV_00456409_Histograms.root", string sampleTag = "Data", bool inGrid=false)
{
    // ### boolean to know which process should be done
    //  Record start time
    gROOT->SetBatch();
    auto start  = std::chrono::steady_clock::now();
    bool isData = false;
    bool isMC   = false;
    ROOT::EnableImplicitMT();

    /// work here if you also want MC
    if (sampleTag == "Data"){
        isData = true;
    }
    else if (sampleTag == "MC"){
        isMC = true;
    }
    else{
        std::cout << "Unknown option for Data/MC" << std::endl;
        return;
    }
    std::cout << "Making Some Distribution for  " << sampleTag << std::endl;

    TFile *myFile=nullptr;
    string eosDirS = "";
    // work only with one file, for MC
    if (inGrid){
        // WIS cluster files
        eosDirS = inputFolder;
        myFile = new TFile(outputFile.c_str(), "RECREATE");
        std::cout << "The output file for histograms: " << outputFile.c_str() << std::endl;
    }
    // work on all files in a folder, for Data
    else{
        // local files

        //////////////////////////////////
        ///// change file path here //////
        //////////////////////////////////

        eosDirS = "/storage/agrp/arkas/MuonWorkingPointFiles/";
        /// local files
        string outputDirS = "HistFiles"; // the output directory
        int status = mkdir(outputDirS.c_str(), 0777);

        /// open the output root file
        myFile = new TFile((outputDirS + "/" + outputFile).c_str(), "RECREATE");
        std::cout << "The output file for histograms: " << (outputDirS + "/" + outputFile).c_str() << std::endl;
        std::cout << "-----------------" << std::endl;
        std::cout << "Processing " << sampleTag << std::endl;
    }
    myFile->cd();


    /// open the histograms
    map<string, ROOT::RDF::TH1DModel> allHisto1Dict;
    /// barrel variables
    /// the key should be a variable which is already added to the DF
    /// the value is the TH1D that we want to create
    allHisto1Dict.insert(make_pair("mll", ROOT::RDF::TH1DModel("dimuon_mass_reco", "dimuon_mass_reco; m_{#mu#mu} [GeV]; Events/bin", 150, 0, 150)));
    allHisto1Dict.insert(make_pair("muon_nprecisionLayers_0", ROOT::RDF::TH1DModel("nprecisionLayers_0", "nprecisionLayers; Number of precision layers (lead); Events/bin", 8, 0, 8)));
    allHisto1Dict.insert(make_pair("muon_nprecisionLayers_1", ROOT::RDF::TH1DModel("nprecisionLayers_1", "nprecisionLayers; Number of precision layers (sub-lead); Events/bin", 8, 0, 8)));
    allHisto1Dict.insert(make_pair("muon_qOverPsignif_0", ROOT::RDF::TH1DModel("muon_qOverPsignif_0", "qOverPsignif; q/p significance (lead); Events/bin", 50, 0, 10)));
    allHisto1Dict.insert(make_pair("muon_qOverPsignif_1", ROOT::RDF::TH1DModel("muon_qOverPsignif_1", "qOverPsignif; q/p significance (sub-lead); Events/bin", 50, 0, 10)));
    allHisto1Dict.insert(make_pair("muon_pt_0", ROOT::RDF::TH1DModel("muon_pt_0", "muon_pt; p_{T} (lead) [GeV]; Events/bin", 50, 0, 500)));
    allHisto1Dict.insert(make_pair("muon_pt_1", ROOT::RDF::TH1DModel("muon_pt_1", "muon_pt; p_{T} (sub-lead) [GeV]; Events/bin", 50, 0, 500)));
    allHisto1Dict.insert(make_pair("muon_phi_0", ROOT::RDF::TH1DModel("muon_phi_0", "muon_phi; #phi (lead); Events/bin", 160, -4, 4)));
    allHisto1Dict.insert(make_pair("muon_phi_1", ROOT::RDF::TH1DModel("muon_phi_1", "muon_phi; #phi (sub-lead); Events/bin", 160, -4, 4)));
    allHisto1Dict.insert(make_pair("muon_eta_0", ROOT::RDF::TH1DModel("muon_eta_0", "muon_eta; #eta (lead); Events/bin", 44, -1.1, 1.1)));
    allHisto1Dict.insert(make_pair("muon_eta_1", ROOT::RDF::TH1DModel("muon_eta_1", "muon_eta; #eta (sub-lead); Events/bin", 44, -1.1, 1.1)));
    
    /// end cap variables
    map<string, ROOT::RDF::TH1DModel> allHisto1Dict_EC;
    allHisto1Dict_EC.insert(make_pair("mll", ROOT::RDF::TH1DModel("dimuon_mass_reco_EC", "dimuon_mass_reco; m_{#mu#mu} [GeV]; Events/bin", 150, 0, 150)));
    allHisto1Dict_EC.insert(make_pair("muon_nprecisionLayers_0", ROOT::RDF::TH1DModel("nprecisionLayers_0_EC", "nprecisionLayers; Number of precision layers (lead); Events/bin", 8, 0, 8)));
    allHisto1Dict_EC.insert(make_pair("muon_nprecisionLayers_1", ROOT::RDF::TH1DModel("nprecisionLayers_1_EC", "nprecisionLayers; Number of precision layers (sub-lead); Events/bin", 8, 0, 8)));
    allHisto1Dict_EC.insert(make_pair("muon_qOverPsignif_0", ROOT::RDF::TH1DModel("muon_qOverPsignif_0_EC", "qOverPsignif; q/p significance (lead); Events/bin", 50, 0, 10)));
    allHisto1Dict_EC.insert(make_pair("muon_qOverPsignif_1", ROOT::RDF::TH1DModel("muon_qOverPsignif_1_EC", "qOverPsignif; q/p significance (sub-lead); Events/bin", 50, 0, 10)));
    allHisto1Dict_EC.insert(make_pair("muon_pt_0", ROOT::RDF::TH1DModel("muon_pt_0_EC", "muon_pt; p_{T} (lead) [GeV]; Events/bin", 50, 0, 500)));
    allHisto1Dict_EC.insert(make_pair("muon_pt_1", ROOT::RDF::TH1DModel("muon_pt_1_EC", "muon_pt; p_{T} (sub-lead) [GeV]; Events/bin", 50, 0, 500)));
    allHisto1Dict_EC.insert(make_pair("muon_phi_0", ROOT::RDF::TH1DModel("muon_phi_0_EC", "muon_phi; #phi (lead); Events/bin", 160, -4, 4)));
    allHisto1Dict_EC.insert(make_pair("muon_phi_1", ROOT::RDF::TH1DModel("muon_phi_1_EC", "muon_phi; #phi (sub-lead); Events/bin", 160, -4, 4)));
    allHisto1Dict_EC.insert(make_pair("muon_eta_0", ROOT::RDF::TH1DModel("muon_eta_0_EC", "muon_eta; #eta (lead); Events/bin", 108, -2.7, 2.7)));
    allHisto1Dict_EC.insert(make_pair("muon_eta_1", ROOT::RDF::TH1DModel("muon_eta_1_EC", "muon_eta; #eta (sub-lead); Events/bin", 108, -2.7, 2.7)));

    
    /// create 2D maps, with manyMaps class
    map<string, manyMaps> allHisto2Dict;
    /// barrel histograms
    /// rememeber the constructor: manyMaps(string keyname_, string key_x_, string key_y_, ROOT::RDF::TH2DModel mod_)
    /// key_x_ and key_y_ should be a variable in DF 
    allHisto2Dict.insert(make_pair("muon_eta_vs_phi_0", manyMaps("muon_eta_vs_phi_0", "muon_phi_0", "muon_eta_0", ROOT::RDF::TH2DModel("muon_eta_vs_phi_0", "muon_eta_vs_phi_0; #phi (lead); #eta (lead)", 160, -4, 4, 44, -1.1, 1.1))));
    allHisto2Dict.insert(make_pair("muon_eta_vs_phi_1", manyMaps("muon_eta_vs_phi_1", "muon_phi_1", "muon_eta_1", ROOT::RDF::TH2DModel("muon_eta_vs_phi_1", "muon_eta_vs_phi_1; #phi (sub-lead); #eta (sub-lead)", 160, -4, 4, 44, -1.1, 1.1))));
    /// end-cap histograms
    map<string, manyMaps> allHisto2Dict_EC;
    allHisto2Dict_EC.insert(make_pair("muon_eta_vs_phi_0", manyMaps("muon_eta_vs_phi_0", "muon_phi_0", "muon_eta_0", ROOT::RDF::TH2DModel("muon_eta_vs_phi_0", "muon_eta_vs_phi_0; #phi (lead); #eta (lead)", 160, -4, 4, 108, -2.7, 2.7))));
    allHisto2Dict_EC.insert(make_pair("muon_eta_vs_phi_1", manyMaps("muon_eta_vs_phi_1", "muon_phi_1", "muon_eta_1", ROOT::RDF::TH2DModel("muon_eta_vs_phi_1", "muon_eta_vs_phi_1; #phi (sub-lead); #eta (sub-lead)", 160, -4, 4, 108, -2.7, 2.7))));


    string treeInS = "analysis";
    string treeInFileS = ""; 

    stringstream ss;
    ss << treeInS;
    TChain chain(ss.str().c_str());

    // work only with one file, this is when we run on WIS cluster (MC)
    if (inGrid){
        treeInFileS = eosDirS;
        cout << "file added: " << treeInFileS << endl;
        chain.Add(treeInFileS.c_str());
    }
    // add all files in a given folder, this is when we run on data
    else{
        size_t numberofFiles = number_of_files_in_directory(eosDirS + "/" + inputFolder);
        cout << "The folder: " << eosDirS + "/" + inputFolder << " has " << numberofFiles << " files." << endl;

        /// loop over all the files in the input directory eosDirS + "/" + inputFolder
        for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{eosDirS + "/" + inputFolder}) 
        {
            treeInFileS = dir_entry.path().string();
            cout << "file added: " << treeInFileS << endl;
            chain.Add(treeInFileS.c_str());
        }
    }
        
    /// create the data frame
    ROOT::RDataFrame d(chain);
    auto count = d.Count();
    // # Determine the number of events to loop over
    unsigned long long rangeNumber = -1;
    rangeNumber = *count;
    // # Start loop over all events
    std::cout << "Looping over " << rangeNumber << " Events" << std::endl;

    /// filter out all events with low number of muons or low pt
    auto dCut = d.Filter("!(muon_pt.size() < 2)")
                 /// add HLT24 trigger cut
                 .Filter("!(muon_pt[0] < 25.)")
                 .Filter("!(muon_pt[1] < 25.)")
                 /// add ivarmedium trigger cut
                 .Filter("muon_ptvarcone30[0]/muon_pt[0] < 0.07")
                 .Filter("muon_ptvarcone30[1]/muon_pt[1] < 0.07")
                 /// selection based on eta cut
                 ///  .Filter("!(std::abs(muon_eta.at(0)) > 1.05)")
                 ///  .Filter("!(std::abs(muon_eta.at(1)) > 1.05)")

                 /// add new columns to dataframe
                 .Define("mll", "getInvariantMass(muon_pt, muon_eta, muon_phi, muon_e)")
                 .Define("muon_nprecisionLayers_0", "muon_nprecisionLayers[0]")
                 .Define("muon_nprecisionLayers_1", "muon_nprecisionLayers[1]")
                 .Define("muon_qOverPsignif_0", "muon_qOverPsignif[0]")
                 .Define("muon_qOverPsignif_1", "muon_qOverPsignif[1]")
                 .Define("ptBin_0", "getPtBin(muon_pt[0])")
                 .Define("ptBin_1", "getPtBin(muon_pt[1])")
                 .Define("etaBin_0", "getEtaBin(muon_eta[0])")
                 .Define("etaBin_1", "getEtaBin(muon_eta[1])")
                 .Define("etaBinEC_0", "getEtaBin_EC(muon_eta[0])")
                 .Define("etaBinEC_1", "getEtaBin_EC(muon_eta[1])")
                 .Define("muon_pt_0", "muon_pt[0]")
                 .Define("muon_pt_1", "muon_pt[1]")
                 .Define("muon_phi_0", "muon_phi[0]")
                 .Define("muon_phi_1", "muon_phi[1]")
                 .Define("muon_eta_0", "muon_eta[0]")
                 .Define("muon_eta_1", "muon_eta[1]")
                 .Define("muon_Medium_0", "muon_Medium[0]")
                 .Define("muon_Medium_1", "muon_Medium[1]")
                 .Define("muon_HighPt_0", "muon_HighPt[0]")
                 .Define("muon_HighPt_1", "muon_HighPt[1]")
                 .Define("muon_LowPt_0", "muon_LowPt[0]")
                 .Define("muon_LowPt_1", "muon_LowPt[1]")
                 .Define("muon_Tight_0", "muon_Tight[0]")
                 .Define("muon_Tight_1", "muon_Tight[1]")
                 /// use of a lambda function
                 .Define("sampleTag", [sampleTag] { return sampleTag; })
                 .Define("weight", "returnWeight(mcEventWeight, sampleTag)");


    /////////////////////////////////////////////////
    /// Barrel region                             ///
    /////////////////////////////////////////////////
    /// define different df based on working point
    /// only selecting the Z-peak
    auto dCutMedium = dCut.Filter("!(std::abs(muon_eta.at(0)) > 1.05) && !(std::abs(muon_eta.at(1)) > 1.05)")
                          .Filter("muon_Medium[0] && muon_Medium[1]")
                          .Filter("80 < mll && mll < 100");

    /// break the events into separate pt and eta bins
    auto dCutMedium_ptBin1  = dCutMedium.Filter("ptBin_0==1");
    auto dCutMedium_ptBin2  = dCutMedium.Filter("ptBin_0==2");
    auto dCutMedium_ptBin3  = dCutMedium.Filter("ptBin_0==3");
    auto dCutMedium_etaBin1 = dCutMedium.Filter("etaBin_0==1");
    auto dCutMedium_etaBin2 = dCutMedium.Filter("etaBin_0==2");
    auto dCutMedium_etaBin3 = dCutMedium.Filter("etaBin_0==3");

    auto dCutHigh   = dCut.Filter("!(std::abs(muon_eta.at(0)) > 1.05) && !(std::abs(muon_eta.at(1)) > 1.05)")
                           .Filter("muon_HighPt[0] && muon_HighPt[1]")
                           .Filter("80 < mll && mll < 100");

    /// break the events into separate pt and eta bins
    auto dCutHigh_ptBin1  = dCutHigh.Filter("ptBin_0==1");
    auto dCutHigh_ptBin2  = dCutHigh.Filter("ptBin_0==2");
    auto dCutHigh_ptBin3  = dCutHigh.Filter("ptBin_0==3");
    auto dCutHigh_etaBin1 = dCutHigh.Filter("etaBin_0==1");
    auto dCutHigh_etaBin2 = dCutHigh.Filter("etaBin_0==2");
    auto dCutHigh_etaBin3 = dCutHigh.Filter("etaBin_0==3");

    // auto dCutLow    = dCut.Filter("!(std::abs(muon_eta.at(0)) > 1.05) && !(std::abs(muon_eta.at(1)) > 1.05)")
    //                       .Filter("muon_LowPt[0] && muon_LowPt[1]")
    //                       .Filter("80 < mll && mll < 100");

    // auto dCutTight  = dCut.Filter("!(std::abs(muon_eta.at(0)) > 1.05) && !(std::abs(muon_eta.at(1)) > 1.05)")
    //                       .Filter("muon_Tight[0] && muon_Tight[1]")
    //                       .Filter("80 < mll && mll < 100");


    /// map to store histograms
    map<string, TH1D*> prepared1DHistogram;
    map<string, TH2D*> prepared2DHistogram;

    //////////////////////////////
    /// medium working point /////
    //////////////////////////////
    prepare1DHistogram(dCutMedium, "medium", allHisto1Dict, prepared1DHistogram);
    prepare2DHistogram(dCutMedium, "medium", allHisto2Dict, prepared2DHistogram);

    /// getting binned distributions
    // pt bins
    prepare1DHistogram(dCutMedium_ptBin1, "medium_ptBin1", allHisto1Dict, prepared1DHistogram);
    prepare2DHistogram(dCutMedium_ptBin1, "medium_ptBin1", allHisto2Dict, prepared2DHistogram);

    prepare1DHistogram(dCutMedium_ptBin2, "medium_ptBin2", allHisto1Dict, prepared1DHistogram);
    prepare2DHistogram(dCutMedium_ptBin2, "medium_ptBin2", allHisto2Dict, prepared2DHistogram);

    prepare1DHistogram(dCutMedium_ptBin3, "medium_ptBin3", allHisto1Dict, prepared1DHistogram);
    prepare2DHistogram(dCutMedium_ptBin3, "medium_ptBin3", allHisto2Dict, prepared2DHistogram);

    /// eta bins
    prepare1DHistogram(dCutMedium_etaBin1, "medium_etaBin1", allHisto1Dict, prepared1DHistogram);
    prepare2DHistogram(dCutMedium_etaBin1, "medium_etaBin1", allHisto2Dict, prepared2DHistogram);

    prepare1DHistogram(dCutMedium_etaBin2, "medium_etaBin2", allHisto1Dict, prepared1DHistogram);
    prepare2DHistogram(dCutMedium_etaBin2, "medium_etaBin2", allHisto2Dict, prepared2DHistogram);

    prepare1DHistogram(dCutMedium_etaBin3, "medium_etaBin3", allHisto1Dict, prepared1DHistogram);
    prepare2DHistogram(dCutMedium_etaBin3, "medium_etaBin3", allHisto2Dict, prepared2DHistogram);
    
    
    //////////////////////////////
    /// high-pt working point ////
    //////////////////////////////
    prepare1DHistogram(dCutHigh, "highpt", allHisto1Dict, prepared1DHistogram);
    prepare2DHistogram(dCutHigh, "highpt", allHisto2Dict, prepared2DHistogram);


    /// getting binned distributions
    // pt bins
    prepare1DHistogram(dCutHigh_ptBin1, "highpt_ptBin1", allHisto1Dict, prepared1DHistogram);
    prepare2DHistogram(dCutHigh_ptBin1, "highpt_ptBin1", allHisto2Dict, prepared2DHistogram);

    prepare1DHistogram(dCutHigh_ptBin2, "highpt_ptBin2", allHisto1Dict, prepared1DHistogram);
    prepare2DHistogram(dCutHigh_ptBin2, "highpt_ptBin2", allHisto2Dict, prepared2DHistogram);

    prepare1DHistogram(dCutHigh_ptBin3, "highpt_ptBin3", allHisto1Dict, prepared1DHistogram);
    prepare2DHistogram(dCutHigh_ptBin3, "highpt_ptBin3", allHisto2Dict, prepared2DHistogram);

    /// eta bins
    prepare1DHistogram(dCutHigh_etaBin1, "highpt_etaBin1", allHisto1Dict, prepared1DHistogram);
    prepare2DHistogram(dCutHigh_etaBin1, "highpt_etaBin1", allHisto2Dict, prepared2DHistogram);

    prepare1DHistogram(dCutHigh_etaBin2, "highpt_etaBin2", allHisto1Dict, prepared1DHistogram);
    prepare2DHistogram(dCutHigh_etaBin2, "highpt_etaBin2", allHisto2Dict, prepared2DHistogram);
    
    prepare1DHistogram(dCutHigh_etaBin3, "highpt_etaBin3", allHisto1Dict, prepared1DHistogram);
    prepare2DHistogram(dCutHigh_etaBin3, "highpt_etaBin3", allHisto2Dict, prepared2DHistogram);


    /////////////////////////////////////////////////
    // End cap region                             ///
    /////////////////////////////////////////////////
    /// define different df based on working point

    auto dCutMedium_EC = dCut.Filter("!(std::abs(muon_eta.at(0)) < 1.3) && !(std::abs(muon_eta.at(1)) < 1.3)")
                             .Filter("muon_Medium[0] && muon_Medium[1]")
                             .Filter("80 < mll && mll < 100");

    /// break the events into separate pt and eta bins
    auto dCutMedium_EC_ptBin1  = dCutMedium_EC.Filter("ptBin_0==1");
    auto dCutMedium_EC_ptBin2  = dCutMedium_EC.Filter("ptBin_0==2");
    auto dCutMedium_EC_ptBin3  = dCutMedium_EC.Filter("ptBin_0==3");
    auto dCutMedium_EC_etaBin1 = dCutMedium_EC.Filter("etaBinEC_0==1");
    auto dCutMedium_EC_etaBin2 = dCutMedium_EC.Filter("etaBinEC_0==2");
    auto dCutMedium_EC_etaBin3 = dCutMedium_EC.Filter("etaBinEC_0==3");



    auto dCutHigh_EC   = dCut.Filter("!(std::abs(muon_eta.at(0)) < 1.3) && !(std::abs(muon_eta.at(1)) < 1.3)")
                             .Filter("muon_HighPt[0] && muon_HighPt[1]")
                             .Filter("80 < mll && mll < 100");

    /// break the events into separate pt and eta bins
    auto dCutHigh_EC_ptBin1  = dCutHigh_EC.Filter("ptBin_0==1");
    auto dCutHigh_EC_ptBin2  = dCutHigh_EC.Filter("ptBin_0==2");
    auto dCutHigh_EC_ptBin3  = dCutHigh_EC.Filter("ptBin_0==3");
    auto dCutHigh_EC_etaBin1 = dCutHigh_EC.Filter("etaBinEC_0==1");
    auto dCutHigh_EC_etaBin2 = dCutHigh_EC.Filter("etaBinEC_0==2");
    auto dCutHigh_EC_etaBin3 = dCutHigh_EC.Filter("etaBinEC_0==3");


    /// map to store histograms
    map<string, TH1D*> prepared1DHistogram_EC;
    map<string, TH2D*> prepared2DHistogram_EC;

    //////////////////////////////
    /// medium working point /////
    //////////////////////////////
    prepare1DHistogram(dCutMedium_EC, "EC_medium", allHisto1Dict_EC, prepared1DHistogram_EC);
    prepare2DHistogram(dCutMedium_EC, "EC_medium", allHisto2Dict_EC, prepared2DHistogram_EC);

    /// use pt bin histograms
    prepare1DHistogram(dCutMedium_EC_ptBin1, "EC_medium_ptBin1", allHisto1Dict_EC, prepared1DHistogram_EC);
    prepare2DHistogram(dCutMedium_EC_ptBin1, "EC_medium_ptBin1", allHisto2Dict_EC, prepared2DHistogram_EC);

    prepare1DHistogram(dCutMedium_EC_ptBin2, "EC_medium_ptBin2", allHisto1Dict_EC, prepared1DHistogram_EC);
    prepare2DHistogram(dCutMedium_EC_ptBin2, "EC_medium_ptBin2", allHisto2Dict_EC, prepared2DHistogram_EC);

    prepare1DHistogram(dCutMedium_EC_ptBin3, "EC_medium_ptBin3", allHisto1Dict_EC, prepared1DHistogram_EC);
    prepare2DHistogram(dCutMedium_EC_ptBin3, "EC_medium_ptBin3", allHisto2Dict_EC, prepared2DHistogram_EC);

    /// eta bins
    prepare1DHistogram(dCutMedium_EC_etaBin1, "EC_medium_etaBin1", allHisto1Dict_EC, prepared1DHistogram_EC);
    prepare2DHistogram(dCutMedium_EC_etaBin1, "EC_medium_etaBin1", allHisto2Dict_EC, prepared2DHistogram_EC);

    prepare1DHistogram(dCutMedium_EC_etaBin2, "EC_medium_etaBin2", allHisto1Dict_EC, prepared1DHistogram_EC);
    prepare2DHistogram(dCutMedium_EC_etaBin2, "EC_medium_etaBin2", allHisto2Dict_EC, prepared2DHistogram_EC);

    prepare1DHistogram(dCutMedium_EC_etaBin3, "EC_medium_etaBin3", allHisto1Dict_EC, prepared1DHistogram_EC);
    prepare2DHistogram(dCutMedium_EC_etaBin3, "EC_medium_etaBin3", allHisto2Dict_EC, prepared2DHistogram_EC);

    
    //////////////////////////////
    /// high-pt working point ////
    //////////////////////////////
    prepare1DHistogram(dCutHigh_EC, "EC_highpt", allHisto1Dict_EC, prepared1DHistogram_EC);
    prepare2DHistogram(dCutHigh_EC, "EC_highpt", allHisto2Dict_EC, prepared2DHistogram_EC);

    /// pt bins
    prepare1DHistogram(dCutHigh_EC_ptBin1, "EC_highpt_ptBin1", allHisto1Dict_EC, prepared1DHistogram_EC);
    prepare2DHistogram(dCutHigh_EC_ptBin1, "EC_highpt_ptBin1", allHisto2Dict_EC, prepared2DHistogram_EC);

    prepare1DHistogram(dCutHigh_EC_ptBin2, "EC_highpt_ptBin2", allHisto1Dict_EC, prepared1DHistogram_EC);
    prepare2DHistogram(dCutHigh_EC_ptBin2, "EC_highpt_ptBin2", allHisto2Dict_EC, prepared2DHistogram_EC);

    prepare1DHistogram(dCutHigh_EC_ptBin3, "EC_highpt_ptBin3", allHisto1Dict_EC, prepared1DHistogram_EC);
    prepare2DHistogram(dCutHigh_EC_ptBin3, "EC_highpt_ptBin3", allHisto2Dict_EC, prepared2DHistogram_EC);

    /// eta bins
    prepare1DHistogram(dCutHigh_EC_etaBin1, "EC_highpt_etaBin1", allHisto1Dict_EC, prepared1DHistogram_EC);
    prepare2DHistogram(dCutHigh_EC_etaBin1, "EC_highpt_etaBin1", allHisto2Dict_EC, prepared2DHistogram_EC);

    prepare1DHistogram(dCutHigh_EC_etaBin2, "EC_highpt_etaBin2", allHisto1Dict_EC, prepared1DHistogram_EC);
    prepare2DHistogram(dCutHigh_EC_etaBin2, "EC_highpt_etaBin2", allHisto2Dict_EC, prepared2DHistogram_EC);

    prepare1DHistogram(dCutHigh_EC_etaBin3, "EC_highpt_etaBin3", allHisto1Dict_EC, prepared1DHistogram_EC);
    prepare2DHistogram(dCutHigh_EC_etaBin3, "EC_highpt_etaBin3", allHisto2Dict_EC, prepared2DHistogram_EC);

    /////////////////////////////////////////////////
    // Save histograms                            ///
    /////////////////////////////////////////////////
    /// write the histograms to the root file, barrel
    for (map<string, TH1D*>::iterator it = prepared1DHistogram.begin(); it != prepared1DHistogram.end(); ++it){
        it->second->Write(it->first.c_str());
    }
    for (map<string, TH2D*>::iterator it = prepared2DHistogram.begin(); it != prepared2DHistogram.end(); ++it){
        it->second->Write(it->first.c_str());
    }
    /// write the histograms to the root file, endcap
    for (map<string, TH1D*>::iterator it = prepared1DHistogram_EC.begin(); it != prepared1DHistogram_EC.end(); ++it){
        it->second->Write(it->first.c_str());
    }
    for (map<string, TH2D*>::iterator it = prepared2DHistogram_EC.begin(); it != prepared2DHistogram_EC.end(); ++it){
        it->second->Write(it->first.c_str());
    }
    myFile->Close();
    
    // Record end time
    auto finish = std::chrono::steady_clock::now();
    auto diff = finish - start;
    std::cout << "Elapsed time : " << chrono::duration<double, milli>(diff).count() / 1000.0 << " s" << endl;
    delete myFile;
}
