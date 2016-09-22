#ifndef TEST_H
#define TEST_H

#include <string>
#include <vector>
#include <iostream>

#include "art/Framework/Core/ModuleMacros.h" 
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Handle.h" 
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "lardataobj/RawData/RawDigit.h"

#include "WireCellSigProc/OmnibusNoiseFilter.h"

namespace testalg {

  class Test : public art::EDAnalyzer {

  public:
    explicit Test(fhicl::ParameterSet const& pset);
    virtual ~Test();

    void analyze(art::Event const& evt);
    void reconfigure(fhicl::ParameterSet const& pset);
    
    void beginJob();
    void endJob();

    //likely we will need begin/end run and subrun functions
    void beginRun(art::Run const& run);
    void endRun(art::Run const& run);
    void beginSubRun(art::SubRun const& subrun);
    void endSubRun(art::SubRun const& subrun);
    
  private:

    //******************************
    //Variables Taken from FHICL File
    //std::string       fRawDigitModuleLabel;   //label for rawdigit module
  }; //end class Noise


  //-------------------------------------------------------------------
  Test::Test(fhicl::ParameterSet const& pset)
    : EDAnalyzer(pset){ 
    this->reconfigure(pset); 
  }

  //-------------------------------------------------------------------
  Test::~Test(){}

  //-------------------------------------------------------------------
  void Test::reconfigure(fhicl::ParameterSet const& pset){
    //fRawDigitModuleLabel = pset.get<std::string>("RawDigitModuleLabel");
  }

  //-------------------------------------------------------------------
  void Test::beginJob(){
    //art::ServiceHandle<art::TFileService> tfs;
    //int fMaxTicks = 9594;
  }

  //-------------------------------------------------------------------
  void Test::endJob(){
  }

  //-------------------------------------------------------------------
  void Test::beginRun(art::Run const& run){
    //art::ServiceHandle<art::TFileService> tfs;
  }

  //-------------------------------------------------------------------
  void Test::endRun(art::Run const& run){
    //art::ServiceHandle<art::TFileService> tfs;
    //return;
  }

  //-------------------------------------------------------------------
  void Test::beginSubRun(art::SubRun const& subrun){
  }

  //-------------------------------------------------------------------
  void Test::endSubRun(art::SubRun const& subrun){
  }
  
  //-------------------------------------------------------------------
  void Test::analyze(art::Event const& evt){
    std::cout << evt.event() << std::endl;
    //art::ServiceHandle<art::TFileService> tfs;    
    //art::Handle< std::vector<raw::RawDigit> > rawDigitHandle;
    //evt.getByLabel(fRawDigitModuleLabel,rawDigitHandle);
    //std::vector<raw::RawDigit> const& rawDigitVector(*rawDigitHandle);
    //const unsigned int n_channels = rawDigitVector.size();
  }
  
  DEFINE_ART_MODULE(Test)

} //end namespace testalg

#endif //TEST_H
