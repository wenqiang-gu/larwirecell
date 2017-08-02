#include "CookedFrameSink.h"
//#include "art/Framework/Principal/Handle.h" 

#include "lardataobj/RecoBase/Wire.h"

#include "art/Framework/Principal/Event.h"
#include "art/Framework/Core/EDProducer.h"

#include "WireCellIface/IFrame.h"
#include "WireCellIface/ITrace.h"
#include "WireCellUtil/NamedFactory.h"

WIRECELL_FACTORY(wclsCookedFrameSink, wcls::CookedFrameSink, wcls::IArtEventVisitor, WireCell::IFrameSink)


using namespace wcls;
using namespace WireCell;

CookedFrameSink::CookedFrameSink()
    : m_frame(nullptr)
{
}

CookedFrameSink::~CookedFrameSink()
{
}


WireCell::Configuration CookedFrameSink::default_configuration() const
{
    Configuration cfg;
    cfg["anode"] = "AnodePlane"; 
    // frames to output
    cfg["frame_tags"][0] = "gauss";
    cfg["frame_tags"][1] = "wiener";
    return cfg;
}

void CookedFrameSink::configure(const WireCell::Configuration& cfg)
{
    const std::string anode_tn = cfg["anode"].asString();
    if (anode_tn.empty()) {
        THROW(ValueError() << errmsg{"CookedFrameSink requires an anode plane"});
    }

    // this throws if not found
    m_anode = Factory::find_tn<IAnodePlane>(anode_tn);

    auto jtags = cfg["frame_tags"];
    std::cerr << "CookedFrameSink: saving " << jtags.size() << " tags\n";
    for (auto jtag : jtags) {
	std::string tag = jtag.asString();
	std::cerr << "\t" << tag << "\n";
        m_frame_tags.push_back(tag);
    }

}

void CookedFrameSink::produces(art::EDProducer* prod)
{
    assert(prod);
    //prod->produces< std::vector<recob::Wire> >("CookedFrame");

    for (auto tag : m_frame_tags) {
	std::cerr << "CookedFrameSink: promising to produce recob::Wires named \"" << tag << "\"\n";
	prod->produces< std::vector<recob::Wire> >(tag);
    }
}

// fixme: this is copied from sigproc/FrameUtil but it's currently private code.  
static
ITrace::vector tagged_traces(IFrame::pointer frame, IFrame::tag_t tag)
{
    ITrace::vector ret;
    auto const& all_traces = frame->traces();
    for (size_t index : frame->tagged_traces(tag)) {
        ret.push_back(all_traces->at(index));
    }
    if (!ret.empty()) {
        return ret;
    }
    auto ftags = frame->frame_tags();
    if (std::find(ftags.begin(), ftags.end(), tag) == ftags.end()) {
        return ret;
    }
    return *all_traces;		// must make copy of shared pointers
}


void CookedFrameSink::visit(art::Event & event)
{
    if (!m_frame) {
        std::cerr << "CookedFrameSink: I have no frame to save to art::Event\n";
        return;
    }

    std::cerr << "CookedFrameSink: got " << m_frame->traces()->size() << " total traces\n";

    for (auto tag : m_frame_tags) {

	auto traces = tagged_traces(m_frame, tag);
	if (traces.empty()) {
	    std::cerr << "CookedFrameSink: no traces for tag \"" << tag << "\"\n";
	    continue;
	}


	std::unique_ptr<std::vector<recob::Wire> > outwires(new std::vector<recob::Wire>);

	// what about the frame's time() and ident()?
      
	for (const auto& trace : traces) {
	    
	    const int tbin = trace->tbin();
	    const int chid = trace->channel();
	    const auto& charge = trace->charge();

	    //std::cerr << tag << ": chid=" << chid << " tbin=" << tbin << " nq=" << charge.size() << std::endl;

	    // this is a lar::sparse_vector<float>
	    recob::Wire::RegionsOfInterest_t roi(charge, tbin);

	    // geo::kU, geo::kV, geo::kW
	    auto wpid = m_anode->resolve(chid);
	    geo::View_t view;
	    switch(wpid.layer()) {
	    case WireCell::kUlayer:
		view = geo::kU;
		break;
	    case WireCell::kVlayer:
		view = geo::kV;
		break;
	    case WireCell::kWlayer:
		view = geo::kW;
		break;
	    default:
		view = geo::kUnknown;
	    }
	    
	    // what about those pesky channel map masks?
	    // they are dropped for now.

	    outwires->emplace_back(recob::Wire(roi, chid, view));
	}
	std::cerr << "CookedFrameSink saving " << outwires->size() << " recob::Wires named \""<<tag<<"\"\n";
	event.put(std::move(outwires), tag);
    }

    m_frame = nullptr;
}

bool CookedFrameSink::operator()(const WireCell::IFrame::pointer& frame)
{
    // set an IFrame based on last visited event.
    m_frame = frame;
    return true;
}

// Local Variables:
// mode: c++
// c-basic-offset: 4
// End:
