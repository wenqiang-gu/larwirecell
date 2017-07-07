#include "RawFrameSource.h"
#include "art/Framework/Principal/Handle.h" 

// for tick
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"
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


// is this a good idea?
static
double tdiff(const art::Timestamp& ts1, const art::Timestamp& ts2)
{
    TTimeStamp tts1(ts1.timeHigh(), ts1.timeLow());
    TTimeStamp tts2(ts2.timeHigh(), ts2.timeLow());
    return tts2.AsDouble() - tts1.AsDouble();
}

void RawFrameSource::visit(art::Event & event)
{
    const detinfo::DetectorProperties&   detprop = *lar::providerFrom<detinfo::DetectorPropertiesService>();
    const double tick = detprop.SamplingRate(); // 0.5 * units::microsecond;

    art::Handle< std::vector<raw::RawDigit> > rdvh;

    // fixme: make name configurable
    const std::string data_source = "raw::RawDigits_wcNoiseFilter__DataRecoStage1";
    event.getByLabel(data_source, rdvh); 
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
