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

WIRECELL_FACTORY(wclsRawFrameSource, wcls::RawFrameSource,
		 wcls::IArtEventVisitor, WireCell::IFrameSource)


using namespace wcls;
using namespace WireCell;

RawFrameSource::RawFrameSource()
    : m_frame(nullptr)
    , m_nticks(0)
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
    cfg["frame_tags"][0] = "orig"; // the tags to apply to this frame
    cfg["nticks"] = m_nticks; // if nonzero, truncate or baseline-pad frame to this number of ticks.
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
    for (auto jtag : cfg["frame_tags"]) {
        m_frame_tags.push_back(jtag.asString());
    }
    m_nticks = get(cfg, "nticks", m_nticks);
    // std::cerr << "RawFrameSource: source is \"" << m_source
    //           << "\", tick is " << m_tick/WireCell::units::us << " us "
    // 	      << "nticks=" << m_nticks << std::endl;
    // std::cerr << cfg << std::endl;
}



// is this the right way to diff an art::Timestamp?  
static
double tdiff(const art::Timestamp& ts1, const art::Timestamp& ts2)
{
    TTimeStamp tts1(ts1.timeHigh(), ts1.timeLow());
    TTimeStamp tts2(ts2.timeHigh(), ts2.timeLow());
    return tts2.AsDouble() - tts1.AsDouble();
}


static
SimpleTrace* make_trace(const raw::RawDigit& rd, unsigned int nticks)
{
    const int chid = rd.Channel();
    const int tbin = 0;
    const raw::RawDigit::ADCvector_t& adcv = rd.ADCs();

    short baseline = 0;
    size_t npad=0, nsamples = adcv.size();
    if (nticks) {		// enforce a waveform size
	if (nticks > nsamples) { // need to pad
	    baseline = Waveform::most_frequent(adcv);
	    npad = nticks - nsamples;
	}
	nsamples = nticks;
    }

    auto strace = new SimpleTrace(chid, tbin, nsamples);
    for (size_t itick=0; itick < nsamples; ++ itick) {
	strace->charge()[itick] = adcv[itick];
    }
    for (size_t itick = nsamples; itick < nsamples + npad; ++itick) {
	strace->charge()[itick] = baseline;
    }
    return strace;
}

void RawFrameSource::visit(art::Event & event)
{

    //const detinfo::DetectorProperties&   detprop = *lar::providerFrom<detinfo::DetectorPropertiesService>();
    //const double tick = detprop.SamplingRate(); // 0.5 * units::microsecond;
    const double tick = m_tick; // fixme: want to avoid depending on DetectorPropertiesService for now.

    //std::cerr << "RawFrameSource getting: RawDigits at: \"" << m_source << "\"\n";
    art::Handle< std::vector<raw::RawDigit> > rdvh;
    bool okay = event.getByLabel(m_source, rdvh);
    if (!okay || rdvh->size() == 0) {
        std::string msg = "RawFrameSource failed to get raw::RawDigits: " + m_source;
        std::cerr << msg << std::endl;
        THROW(RuntimeError() << errmsg{msg});
    }
    const std::vector<raw::RawDigit>& rdv(*rdvh);
    const size_t nchannels = rdv.size();
    std::cerr << "RawFrameSource: got " << nchannels << " raw::RawDigit objects\n";

    WireCell::ITrace::vector traces(nchannels);
    for (size_t ind=0; ind<nchannels; ++ind) {
        auto const& rd = rdv.at(ind);
        traces[ind] = ITrace::pointer(make_trace(rd, m_nticks));
	if (!ind) {
	    std::cerr << "\tnticks=" << rd.ADCs().size() << " setting to " << m_nticks << std::endl;
	}
    }

    const double time = tdiff(event.getRun().beginTime(), event.time());
    auto sframe = new WireCell::SimpleFrame(event.event(), time, traces, tick);
    for (auto tag : m_frame_tags) {
        std::cerr << "\ttagged: " << tag << std::endl;
        sframe->tag_frame(tag);
    }
    m_frame = WireCell::IFrame::pointer(sframe);
    
}

bool RawFrameSource::operator()(WireCell::IFrame::pointer& frame)
{
    // set an IFrame based on last visited event.
    frame = m_frame;
    m_frame = nullptr;
    return true;
}
// Local Variables:
// mode: c++
// c-basic-offset: 4
// End:
