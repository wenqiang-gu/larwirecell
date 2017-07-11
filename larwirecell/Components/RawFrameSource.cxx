#include "RawFrameSource.h"
#include "art/Framework/Principal/Handle.h" 

// for tick
//#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"
#include "lardataobj/RawData/RawDigit.h"
#include "art/Framework/Principal/Event.h"

#include "TTimeStamp.h"


#include "WireCellIface/SimpleFrame.h"
#include "WireCellIface/SimpleTrace.h"
#include "WireCellUtil/NamedFactory.h"

WIRECELL_FACTORY(wclsRawFrameSource, wcls::RawFrameSource, wcls::IArtEventVisitor, WireCell::IFrameSource)


using namespace wcls;
using namespace WireCell;

RawFrameSource::RawFrameSource()
    : m_frame(nullptr)
{
}

RawFrameSource::~RawFrameSource()
{
}


WireCell::Configuration RawFrameSource::default_configuration() const
{
    Configuration cfg;
    cfg["source_label"] = "daq"; 
    cfg["tick"] = 0.5*WireCell::units::us;
    return cfg;
}

void RawFrameSource::configure(const WireCell::Configuration& cfg)
{
    const std::string sl = cfg["source_label"].asString();
    if (sl.empty()) {
        THROW(ValueError() << errmsg{"RawFrameSource requires a source_label"});
    }
    m_source = sl;
    m_tick = cfg["tick"].asDouble();
}



// is this the right way to diff an art::Timestamp?  
static
double tdiff(const art::Timestamp& ts1, const art::Timestamp& ts2)
{
    TTimeStamp tts1(ts1.timeHigh(), ts1.timeLow());
    TTimeStamp tts2(ts2.timeHigh(), ts2.timeLow());
    return tts2.AsDouble() - tts1.AsDouble();
}

void RawFrameSource::visit(art::Event & event)
{

    //const detinfo::DetectorProperties&   detprop = *lar::providerFrom<detinfo::DetectorPropertiesService>();
    //const double tick = detprop.SamplingRate(); // 0.5 * units::microsecond;
    const double tick = m_tick; // fixme: want to avoid depending on DetectorPropertiesService for now.

    std::cerr << "RawFrameSource getting: RawDigits at: \"" << m_source << "\"\n";
    art::Handle< std::vector<raw::RawDigit> > rdvh;
    bool okay = event.getByLabel(m_source, rdvh);
    std::cerr << "Result of raw digits: " << okay << std::endl;
    if (rdvh->size() == 0) {
        std::cerr << "EMPTY RAW DIGITS VECTOR\n";
    }
    const std::vector<raw::RawDigit>& rdv(*rdvh);

    const size_t nchannels = rdv.size();

    WireCell::ITrace::vector traces(nchannels);
    for (size_t ind=0; ind<nchannels; ++ind) {
        const auto& rd = rdv.at(ind);
        const int chid = rd.Channel();
        const int tbin = 0;
        const auto& adcs = rd.ADCs();

        WireCell::ITrace::ChargeSequence charges(adcs.size());
        std::transform(adcs.begin(), adcs.begin(), charges.begin(),
                       [](auto& adcVal){return float(adcVal);});
        traces[ind] = std::make_shared<SimpleTrace>(chid, tbin, charges);
    }

    const double time = tdiff(event.getRun().beginTime(), event.time());
    m_frame = std::make_shared<WireCell::SimpleFrame>(event.event(), time, traces, tick);
    
}

bool RawFrameSource::operator()(WireCell::IFrame::pointer& frame)
{
    // set an IFrame based on last visited event.
    frame = m_frame;
    return true;
}
