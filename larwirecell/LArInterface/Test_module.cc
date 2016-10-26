#ifndef TEST_H
#define TEST_H

#include <string>
#include <vector>
#include <iostream>

// Framework includes
#include "art/Framework/Principal/Event.h"
//#include "fhiclcpp/ParameterSet.h"

#include "art/Framework/Core/ModuleMacros.h" 
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Handle.h" 
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "lardataobj/RawData/RawDigit.h"
#include <numeric>		// iota
#include "WireCellIface/SimpleFrame.h"
#include "WireCellIface/SimpleTrace.h"
//#include "TestTrace.h"
#include "WireCellSigProc/OmnibusNoiseFilter.h"
#include "WireCellSigProc/OneChannelNoise.h"
#include "WireCellSigProc/CoherentNoiseSub.h"
#include "WireCellSigProc/SimpleChannelNoiseDB.h"

using namespace WireCell;
//using namespace WireCellIFace;
using namespace std;

//class TestTrace;

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
    //    art::ServiceHandle<util::SignalShapingServiceMicroBooNE> sss;
    //    art::ServiceHandle<util::LArWireCellNoiseFilterService> larWireCellNF;
    //  larWireCellNF->print(17);
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
    art::Handle< std::vector<raw::RawDigit> > rawDigitHandle;
    evt.getByLabel("daq",rawDigitHandle);
    std::vector<raw::RawDigit> const& rawDigitVector(*rawDigitHandle);
    const unsigned int n_channels = rawDigitVector.size();
    std::cout << n_channels << std::endl;

    // S&C microboone sampling parameter database
    const double tick = 0.5*units::microsecond;
    const int nsamples = 9594;

    // Q&D microboone channel map
    std::vector<int> uchans(2400), vchans(2400), wchans(3456);
    const int nchans = uchans.size() + vchans.size() + wchans.size();
    std::iota(uchans.begin(), uchans.end(), 0);
    std::iota(vchans.begin(), vchans.end(), vchans.size());
    std::iota(wchans.begin(), wchans.end(), vchans.size() + uchans.size());

    // Q&D nominal baseline
    const double unombl=2048.0, vnombl=2048.0, wnombl=400.0;

    // Q&D miss-configured channel database
    vector<int> miscfgchan;
    const double from_gain_mVfC=7.8, to_gain_mVfC=14.0,
	from_shaping=1.0*units::microsecond, to_shaping=2.0*units::microsecond;
    for (int ind=2016; ind<= 2095; ++ind) { miscfgchan.push_back(ind); }
    for (int ind=2192; ind<= 2303; ++ind) { miscfgchan.push_back(ind); }
    for (int ind=2352; ind< 2400; ++ind) { miscfgchan.push_back(ind); }

    // hard-coded bad channels
    vector<int> bad_channels;
    for (unsigned int i=0;i!=wchans.size();i++){
      if (i>=7136 - 4800 && i <=7263 - 4800){
	if (i != 7200- 4800 && i!=7215 - 4800)
	  bad_channels.push_back(i+4800);
      }
    }

    // Q&D RC+RC time constant - all have same.
    const double rcrc = 1.0*units::millisecond;
    vector<int> rcrcchans(nchans);
    std::iota(rcrcchans.begin(), rcrcchans.end(), 0);
    
    //harmonic noises
    vector<int> harmonicchans(uchans.size() + vchans.size());
    std::iota(harmonicchans.begin(), harmonicchans.end(), 0);
    
    vector<int> special_chans;
    special_chans.push_back(2240);

    WireCellSigProc::SimpleChannelNoiseDB::mask_t h36kHz(0,169,173);
    WireCellSigProc::SimpleChannelNoiseDB::mask_t h108kHz(0,513,516);
    WireCellSigProc::SimpleChannelNoiseDB::mask_t hspkHz(0,17,19);
    WireCellSigProc::SimpleChannelNoiseDB::multimask_t hharmonic;
    hharmonic.push_back(h36kHz);
    hharmonic.push_back(h108kHz);
    WireCellSigProc::SimpleChannelNoiseDB::multimask_t hspecial;
    hspecial.push_back(h36kHz);
    hspecial.push_back(h108kHz);
    hspecial.push_back(hspkHz);

    // do the coherent subtraction
    
    std::vector< std::vector<int> > channel_groups;
    for (unsigned int i=0;i!=172;i++){
    //for (int i=150;i!=151;i++){
      std::vector<int> channel_group;
      for (int j=0;j!=48;j++){
	channel_group.push_back(i*48+j);
      }
      channel_groups.push_back(channel_group);
    }

    auto noise = new WireCellSigProc::SimpleChannelNoiseDB;
    // initialize
    noise->set_sampling(tick, nsamples);
    // set nominal baseline
    noise->set_nominal_baseline(uchans, unombl);
    noise->set_nominal_baseline(vchans, vnombl);
    noise->set_nominal_baseline(wchans, wnombl);
    // set misconfigured channels
    noise->set_gains_shapings(miscfgchan, from_gain_mVfC, to_gain_mVfC, from_shaping, to_shaping);
    // do the RCRC
    noise->set_rcrc_constant(rcrcchans, rcrc);
    // set initial bad channels
    noise->set_bad_channels(bad_channels);
    // set the harmonic filter
    noise->set_filter(harmonicchans,hharmonic);
    noise->set_filter(special_chans,hspecial);
    noise->set_channel_groups(channel_groups);
    
    shared_ptr<WireCell::IChannelNoiseDatabase> noise_sp(noise);

    auto one = new WireCellSigProc::OneChannelNoise;
    one->set_channel_noisedb(noise_sp);
    shared_ptr<WireCell::IChannelFilter> one_sp(one);

    auto many = new WireCellSigProc::CoherentNoiseSub;
    shared_ptr<WireCell::IChannelFilter> many_sp(many);

    WireCellSigProc::OmnibusNoiseFilter bus;
    bus.set_channel_filters({one_sp});
    bus.set_grouped_filters({many_sp});
    bus.set_channel_noisedb(noise_sp);

    if( n_channels == 0 )
        return;

    //load waveforms
    ITrace::vector traces;
    for(unsigned int ich=0; ich<n_channels; ich++){
        const size_t n_samp = rawDigitVector.at(ich).NADC();
        //std::cout << n_samp << std::endl;
        if( n_samp == 0 )
          continue;

	ITrace::ChargeSequence charges;
	//std::vector<float> charges;
	//ITrace charges = new ITrace();
	//charges.channel() = int(ich);
	for( unsigned int s = 0 ; s < n_samp ; s++ ){
	    float q = (float)rawDigitVector.at(ich).ADCs().at(s);
	    //charges.ChargeSequence.push_back(q);
	    charges.push_back(q);
	}
	//TestTrace* st = new TestTrace(int(ich), 0.0, charges);
	WireCell::SimpleTrace* st = new WireCell::SimpleTrace(int(ich), 0.0, charges);
	traces.push_back(ITrace::pointer(st));
	//std::cout <<  charges.ChargeSequence.size() << std::endl;
    }
    std::cout << traces.size() << std::endl;

    /*
    WireCell::SimpleFrame* sf = new WireCell::SimpleFrame(0, 0, traces);
    IFrame::pointer frame = IFrame::pointer(sf);
    
    IFrame::pointer quiet;
    
    cerr << "Removing noise" << endl;
    bus(frame, quiet);
    cerr << "...done" << endl;
    */
  }
  
  DEFINE_ART_MODULE(Test)

} //end namespace testalg

#endif //TEST_H
