#include "CookedFrameSink.h"
//#include "art/Framework/Principal/Handle.h" 

#include "lardataobj/RecoBase/Wire.h"

#include "art/Framework/Principal/Event.h"

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
}


void CookedFrameSink::visit(art::Event & event)
{
    if (!m_frame) {
        std::cerr << "CookedFrameSink: I have no frame to save to art::Event\n";
        return;
    }

    // shouldn't we pre-allocate vector size?
    std::unique_ptr<std::vector<recob::Wire> > outwires(new std::vector<recob::Wire>);

    // what about the frame's time() and ident()?

    auto traces = m_frame->traces();
    for (const auto& trace : (*traces)) {

        const int tbin = trace->tbin();
        const int chid = trace->channel();
        const auto& charge = trace->charge();

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
    std::cerr << "CookedFrameSink saving " << outwires->size() << " recob::Wires\n";
    event.put(std::move(outwires));
}

bool CookedFrameSink::operator()(const WireCell::IFrame::pointer& frame)
{
    // set an IFrame based on last visited event.
    m_frame = frame;
    return true;
}
