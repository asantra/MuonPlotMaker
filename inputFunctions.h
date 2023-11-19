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