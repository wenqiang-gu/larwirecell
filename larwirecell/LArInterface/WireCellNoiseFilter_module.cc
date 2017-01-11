#ifndef WIRECELLNOISEFILTERMODULE_H   // <--- this is not necessary since this file is not included...
#define WIRECELLNOISEFILTERMODULE_H

// Framework includes
#include "art/Framework/Principal/Event.h"

#include "art/Framework/Core/ModuleMacros.h" 
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Principal/Handle.h" 
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "larevt/CalibrationDBI/Interface/DetPedestalService.h"
#include "larevt/CalibrationDBI/Interface/DetPedestalProvider.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusService.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusProvider.h"
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"
#include "larcore/Geometry/Geometry.h"

#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RawData/raw.h"

// The following suggested by Brett to deal with the def clashes
namespace
{
    #undef HEP_SYSTEM_OF_UNITS_H
    #include "WireCellUtil/Units.h"
}
#include "WireCellUtil/Units.h"
#include "WireCellIface/SimpleFrame.h"
#include "WireCellIface/SimpleTrace.h"
#include "WireCellSigProc/OmnibusNoiseFilter.h"
#include "WireCellSigProc/OneChannelNoise.h"
#include "WireCellSigProc/CoherentNoiseSub.h"
#include "WireCellSigProc/SimpleChannelNoiseDB.h"

#include <numeric>		// iota
#include <string>
#include <vector>
#include <iostream>

namespace noisefilteralg {

class WireCellNoiseFilter : public art::EDProducer {

public:
    explicit WireCellNoiseFilter(fhicl::ParameterSet const& pset);
    virtual ~WireCellNoiseFilter();

    void produce(art::Event & evt);
    void reconfigure(fhicl::ParameterSet const& pset);
    
    void beginJob();
    void endJob();

private:
    
    void DoNoiseFilter(const std::vector<raw::RawDigit>&, std::vector<raw::RawDigit>&) const;

    //******************************
    //Variables Taken from FHICL File
    std::string fDigitModuleLabel;     // label for rawdigit module
    bool        fDoNoiseFiltering;     // Allows for a "pass through" mode
    size_t      fNumTicksToDropFront;  // If we are truncating then this is non-zero
    size_t      fWindowSize;           // Number of ticks in the output RawDigit
    
    // services
}; //end class Noise


//-------------------------------------------------------------------
WireCellNoiseFilter::WireCellNoiseFilter(fhicl::ParameterSet const& pset)
    : EDProducer()
{
    this->reconfigure(pset);
    produces<std::vector<raw::RawDigit> >();
}

//-------------------------------------------------------------------
WireCellNoiseFilter::~WireCellNoiseFilter(){}

//-------------------------------------------------------------------
void WireCellNoiseFilter::reconfigure(fhicl::ParameterSet const& pset){
    fDigitModuleLabel    = pset.get<std::string>("DigitModuleLabel",    "daq");
    fDoNoiseFiltering    = pset.get<bool>       ("DoNoiseFiltering",    true );
    fNumTicksToDropFront = pset.get<size_t>     ("NumTicksToDropFront", 2400 );
    fWindowSize          = pset.get<size_t>     ("WindowSize",          6400 );
}

//-------------------------------------------------------------------
void WireCellNoiseFilter::beginJob(){
    art::ServiceHandle<art::TFileService> tfs;
    //art::ServiceHandle<util::LArWireCellNoiseFilterService> larWireCellNF;
    //larWireCellNF->print(17);
}

//-------------------------------------------------------------------
void WireCellNoiseFilter::endJob(){
    art::ServiceHandle<art::TFileService> tfs;
}
  
//-------------------------------------------------------------------
void WireCellNoiseFilter::produce(art::Event & evt)
{
    // Recover services we will need
    const lariov::DetPedestalProvider& pedestalValues = art::ServiceHandle<lariov::DetPedestalService>()->GetPedestalProvider();
    
    art::Handle< std::vector<raw::RawDigit> > rawDigitHandle;
    evt.getByLabel(fDigitModuleLabel,rawDigitHandle);
    
    // Define the output vector (in case we don't do anything)
    std::unique_ptr<std::vector<raw::RawDigit> > filteredRawDigit(new std::vector<raw::RawDigit>);
    
    if (rawDigitHandle.isValid() && rawDigitHandle->size() > 0)
    {
        const std::vector<raw::RawDigit>& rawDigitVector(*rawDigitHandle);
        
        // Make sure we have the correct window size (e.g. window size = 9600 but data is 9595)
        size_t windowSize(std::min(fWindowSize,rawDigitVector.at(0).NADC()));
        
        if (fNumTicksToDropFront + windowSize > rawDigitVector.at(0).NADC())
            throw cet::exception("WireCellNoiseFilter") << "Ticks to drop + windowsize larger than input buffer\n";
        
        if (fDoNoiseFiltering) DoNoiseFilter(rawDigitVector, *filteredRawDigit);
        else
        {
            // Enable truncation
            size_t startBin(fNumTicksToDropFront);
            size_t stopBin(startBin + windowSize);
            
            raw::RawDigit::ADCvector_t outputVector(windowSize);
            
            for(const auto& rawDigit : rawDigitVector)
            {
                if (rawDigit.NADC() < windowSize) continue;
                
                const raw::RawDigit::ADCvector_t& rawAdcVec = rawDigit.ADCs();
                
                unsigned int channel  = rawDigit.Channel();
                float        pedestal = pedestalValues.PedMean(channel);
                
                std::copy(rawAdcVec.begin() + startBin, rawAdcVec.begin() + stopBin, outputVector.begin());
                
                filteredRawDigit->emplace_back( raw::RawDigit( channel , outputVector.size(), outputVector, raw::kNone) );
                filteredRawDigit->back().SetPedestal(pedestal,2.0);
            }
        }
    }

    //filtered raw digits	
    evt.put(std::move(filteredRawDigit));
}
    
void WireCellNoiseFilter::DoNoiseFilter(const std::vector<raw::RawDigit>& inputWaveforms, std::vector<raw::RawDigit>& outputWaveforms) const
{
    // Recover services we will need
    const lariov::ChannelStatusProvider& channelStatus      = art::ServiceHandle<lariov::ChannelStatusService>()->GetProvider();
    const lariov::DetPedestalProvider&   pedestalValues     = art::ServiceHandle<lariov::DetPedestalService>()->GetPedestalProvider();
    const geo::GeometryCore&             geometry           = *lar::providerFrom<geo::Geometry>();
    const detinfo::DetectorProperties&   detectorProperties = *lar::providerFrom<detinfo::DetectorPropertiesService>();
    
    const unsigned int n_channels = inputWaveforms.size();
    
    // S&C microboone sampling parameter database
    const double tick       = detectorProperties.SamplingRate(); // 0.5 * units::microsecond;
    const size_t nsamples   = inputWaveforms.at(0).NADC();
    const size_t windowSize = std::min(fWindowSize,nsamples);
    
    // Q&D microboone channel map
    std::vector<int> uchans(geometry.Nwires(0)), vchans(geometry.Nwires(1)), wchans(geometry.Nwires(2));
    const int nchans = uchans.size() + vchans.size() + wchans.size();
    std::iota(uchans.begin(), uchans.end(), 0);
    std::iota(vchans.begin(), vchans.end(), vchans.size());
    std::iota(wchans.begin(), wchans.end(), vchans.size() + uchans.size());
    
    // Q&D nominal baseline
    const double unombl=2048.0, vnombl=2048.0, wnombl=400.0;
    
    // Q&D miss-configured channel database
    std::vector<int> miscfgchan;
    const double from_gain_mVfC=7.8, to_gain_mVfC=14.0,from_shaping=1.0*units::microsecond, to_shaping=2.0*units::microsecond;
    for (int ind=2016; ind<= 2095; ++ind) { miscfgchan.push_back(ind); }
    for (int ind=2192; ind<= 2303; ++ind) { miscfgchan.push_back(ind); }
    for (int ind=2352; ind<  2400; ++ind) { miscfgchan.push_back(ind); }
    
    // Recover bad channels from the database
    std::vector<int> bad_channels;
    for(int channelIdx=0; channelIdx<nchans; channelIdx++) if (channelStatus.IsBad(channelIdx)) bad_channels.push_back(channelIdx);
    
    // Q&D RC+RC time constant - all have same.
    const double rcrc = 1.0*units::millisecond;
    std::vector<int> rcrcchans(nchans);
    std::iota(rcrcchans.begin(), rcrcchans.end(), 0);
    
    //harmonic noises
    std::vector<int> harmonicchans(uchans.size() + vchans.size());
    std::iota(harmonicchans.begin(), harmonicchans.end(), 0);
    
    std::vector<int> special_chans;
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
    noise->set_sampling(tick, windowSize);
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
    //Define database object
    std::shared_ptr<WireCell::IChannelNoiseDatabase> noise_sp(noise);
    
    auto one = new WireCellSigProc::OneChannelNoise;
    one->set_channel_noisedb(noise_sp);
    std::shared_ptr<WireCell::IChannelFilter> one_sp(one);
    auto many = new WireCellSigProc::CoherentNoiseSub;
    std::shared_ptr<WireCell::IChannelFilter> many_sp(many);
    
    //define noisefilter object
    WireCellSigProc::OmnibusNoiseFilter bus;
    bus.set_channel_filters({one_sp});
    bus.set_grouped_filters({many_sp});
    bus.set_channel_noisedb(noise_sp);
    
    // Enable truncation
    size_t startBin(fNumTicksToDropFront);
    size_t stopBin(startBin + windowSize);
    
    //load waveforms into traces
    WireCell::ITrace::vector traces;
    for(unsigned int ich=0; ich<n_channels; ich++)
    {
        if( inputWaveforms.at(ich).NADC() < nsamples ) continue;
        
        const raw::RawDigit::ADCvector_t& rawAdcVec = inputWaveforms.at(ich).ADCs();
        
        WireCell::ITrace::ChargeSequence charges;
        
        charges.resize(windowSize);
        
        std::transform(rawAdcVec.begin() + startBin, rawAdcVec.begin() + stopBin, charges.begin(), [](auto& adcVal){return float(adcVal);});
        
        unsigned int chan = inputWaveforms.at(ich).Channel();
        WireCell::SimpleTrace* st = new WireCell::SimpleTrace(chan, 0.0, charges);
        traces.push_back(WireCell::ITrace::pointer(st));
    }
    
    //Load traces into frame
    WireCell::SimpleFrame*    sf    = new WireCell::SimpleFrame(0, 0, traces);
    WireCell::IFrame::pointer frame = WireCell::IFrame::pointer(sf);
    WireCell::IFrame::pointer quiet;
    
    //Do filtering
    bus(frame, quiet);
    
    //Output results
    std::vector< short > waveform(windowSize);
    
    auto quiet_traces = quiet->traces();
    for (auto quiet_trace : *quiet_traces.get()) {
        //int tbin = quiet_trace->tbin();
        unsigned int channel = quiet_trace->channel();
        auto quiet_charges = quiet_trace->charge();
        
        // Recover the database version of the pedestal, we'll offset the waveforms so it matches
        float pedestal = pedestalValues.PedMean(channel);
        
        std::transform(quiet_charges.begin(), quiet_charges.end(), waveform.begin(), [pedestal](auto charge){return std::round(charge+pedestal);});
        
        outputWaveforms.emplace_back( raw::RawDigit( channel , waveform.size(), waveform, raw::kNone) );
        outputWaveforms.back().SetPedestal(pedestal,1.75);
    }
    
    return;
}

  
DEFINE_ART_MODULE(WireCellNoiseFilter)

} //end namespace WireCellNoiseFilteralg

#endif //WireCellNoiseFilterMODULE_H
