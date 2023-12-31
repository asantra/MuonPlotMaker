#ifndef INPUTFUNCTIONS_H
#define INPUTFUNCTIONS_H
#endif

#include <iostream>
#include "TString.h"
#include "TCollection.h"
#include "TKey.h"
#include "TClass.h"
#include "TMath.h"
#include <string>
#include <sys/stat.h>
#include "TChain.h"
#include "TTree.h"
#include <algorithm>
#include <time.h>
#include <chrono>  // for high_resolution_clock
#include <sstream>
#include <cmath>
#include <filesystem>

using namespace std;



TH1D *getOverflow(TH1D *h_Sample){
    int bin = h_Sample->GetNbinsX();
    float lastBinValue = h_Sample->GetBinContent(bin);
    float lastBinError = h_Sample->GetBinError(bin);
    
    float lastBinOverflowValue = h_Sample->GetBinContent(bin+1);
    float lastBinOverflowError = h_Sample->GetBinError(bin+1);
    
    float finalValue = lastBinValue + lastBinOverflowValue;
    float finalError = sqrt(lastBinError*lastBinError + lastBinOverflowError*lastBinOverflowError);
    
    h_Sample->SetBinContent(bin, finalValue);
    h_Sample->SetBinContent(bin+1, 0);
    h_Sample->SetBinError(bin, finalError);
    h_Sample->SetBinError(bin+1, 0);
    
    return h_Sample;
}



TH1D *getUnderflow(TH1D *h_Sample){
    float firstBinValue = h_Sample->GetBinContent(1);
    float firstBinError = h_Sample->GetBinError(1);
    
    float firstBinUnderflowValue = h_Sample->GetBinContent(0);
    float firstBinUnderflowError = h_Sample->GetBinError(0);
    
    float finalValue = firstBinValue + firstBinUnderflowValue;
    float finalError = sqrt(firstBinError*firstBinError + firstBinUnderflowError*firstBinUnderflowError);
    
    h_Sample->SetBinContent(1, finalValue);
    h_Sample->SetBinContent(0, 0.0);
    h_Sample->SetBinError(1, finalError);
    h_Sample->SetBinError(0, 0.0);
    
    return h_Sample;
}


double getInvariantMass(ROOT::VecOps::RVec<float> muon_pt, ROOT::VecOps::RVec<float> muon_eta, ROOT::VecOps::RVec<float> muon_phi, ROOT::VecOps::RVec<float> muon_e)
{
    TLorentzVector tlv1;
    TLorentzVector tlv2;

    tlv1.SetPtEtaPhiE(muon_pt.at(0), muon_eta.at(0), muon_phi.at(0), muon_e.at(0));
    tlv2.SetPtEtaPhiE(muon_pt.at(1), muon_eta.at(1), muon_phi.at(1), muon_e.at(1));

    return (tlv1 + tlv2).M();
}

int getMuonSize(ROOT::VecOps::RVec<float> muon_pt)
{
    int nMuon = muon_pt.size();
    // std::cout << "number of muons: " << nMuon << std::endl;
    return nMuon;
}

/*
///  using log binning on a histogram
void BinLogX(TH1 *h)
{
   TAxis *axis = h->GetXaxis();
   int bins = axis->GetNbins();

   Axis_t from = axis->GetXmin();
   Axis_t to = axis->GetXmax();
   Axis_t width = (to - from) / bins;
   Axis_t *new_bins = new Axis_t[bins + 1];

   for (int i = 0; i <= bins; i++) {
     new_bins[i] = TMath::Power(10, from + i * width);
   }
   axis->Set(bins, new_bins);
   delete new_bins;
}
*/

/// prepare many 2D maps with one key
class manyMaps{
  private:
    string keyname;
    string key_y, key_x;
    ROOT::RDF::TH2DModel mod;
    map<string, ROOT::RDF::TH2DModel> string2TH2DMod; 
    map<string, string> string2first;
    map<string, string> string2second;

  public:
    manyMaps(string, string, string, ROOT::RDF::TH2DModel);
    ROOT::RDF::TH2DModel outTH2DModel(string keyname){
        return string2TH2DMod[keyname];
    };
    string outFirst(string keyname){
        return string2first[keyname];
    };
    string outSecond(string keyname){
        return string2second[keyname];
    };
};

manyMaps::manyMaps(string keyname_, string key_x_, string key_y_, ROOT::RDF::TH2DModel mod_) {
  string2TH2DMod.insert(make_pair(keyname_,mod_));
  string2first.insert(make_pair(keyname_,key_x_));
  string2second.insert(make_pair(keyname_,key_y_));
}


/// get the number of files in a directory
std::size_t number_of_files_in_directory(std::filesystem::path path)
{
    std::size_t number_of_files = 0u;
    for (auto const & file : std::filesystem::directory_iterator(path))
    {
        ++number_of_files;
    }
    return number_of_files;
}


/// separate pt bin
int getPtBin(float pt){
    int binNum = -999;
    if(abs(pt) <= 60)binNum = 1;
    else if(abs(pt) > 60 && abs(pt) <= 80)binNum = 2;
    else if(abs(pt) > 80 && abs(pt) <= 100)binNum = 3;
    else if(abs(pt) > 100 && abs(pt) <= 120)binNum = 4;
    else if(abs(pt) > 120 && abs(pt) <= 140)binNum = 5;
    else if(abs(pt) > 140 && abs(pt) <= 160)binNum = 6;
    else if(abs(pt) > 160 && abs(pt) <= 180)binNum = 7;
    else if(abs(pt) > 180 && abs(pt) <= 200)binNum = 8;
    else if(abs(pt) > 200 && abs(pt) <= 220)binNum = 9;
    else binNum = 10;
    return binNum;
}

/// separate eta bin
int getEtaBin(float eta){
    int binNum = -999;
    if(abs(eta) <= 0.1)binNum=1;
    else if(abs(eta) > 0.1 && abs(eta) <= 0.2)binNum=2;
    else if(abs(eta) > 0.2 && abs(eta) <= 0.3)binNum=3;
    else if(abs(eta) > 0.3 && abs(eta) <= 0.4)binNum=4;
    else if(abs(eta) > 0.4 && abs(eta) <= 0.5)binNum=5;
    else if(abs(eta) > 0.5 && abs(eta) <= 0.6)binNum=6;
    else if(abs(eta) > 0.6 && abs(eta) <= 0.7)binNum=7;
    else if(abs(eta) > 0.7 && abs(eta) <= 0.8)binNum=8;
    else if(abs(eta) > 0.8 && abs(eta) <= 0.9)binNum=9;
    else binNum=10;
    return binNum;
}

/// separate eta bin in endcap
int getEtaBin_EC(float eta){
    int binNum = -999;
    if(abs(eta) <= 1.4)binNum=1;
    else if(abs(eta) > 1.4 && abs(eta) <= 1.5)binNum=2;
    else if(abs(eta) > 1.5 && abs(eta) <= 1.6)binNum=3;
    else if(abs(eta) > 1.6 && abs(eta) <= 1.7)binNum=4;
    else if(abs(eta) > 1.7 && abs(eta) <= 1.8)binNum=5;
    else if(abs(eta) > 1.8 && abs(eta) <= 1.9)binNum=6;
    else if(abs(eta) > 1.9 && abs(eta) <= 2.0)binNum=7;
    else if(abs(eta) > 2.0 && abs(eta) <= 2.1)binNum=8;
    else if(abs(eta) > 2.1 && abs(eta) <= 2.2)binNum=9;
    else binNum=10;
    return binNum;
}