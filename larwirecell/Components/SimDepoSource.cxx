#include "SimDepoSource.h"

#include "art/Framework/Principal/Handle.h" 
#include "art/Framework/Principal/Event.h"

#include "lardataobj/Simulation/SimEnergyDeposit.h"

#include "WireCellUtil/NamedFactory.h"
#include "WireCellIface/SimpleDepo.h"

WIRECELL_FACTORY(wclsSimDepoSource, wcls::SimDepoSource, wcls::IArtEventVisitor,
                 WireCell::IDepoSource, WireCell::IConfigurable)


using namespace wcls;

SimDepoSource::SimDepoSource()
{
}

SimDepoSource::~SimDepoSource()
{
}

WireCell::Configuration SimDepoSource::default_configuration() const
{
    WireCell::Configuration cfg;
    return cfg;
}
void SimDepoSource::configure(const WireCell::Configuration& cfg)
{
}

const std::string label = "bogus"; // fixme:make configurable
void SimDepoSource::visit(art::Event & event)
{
    art::Handle< std::vector<sim::SimEnergyDeposit> > sedvh;

    bool okay = event.getByLabel(label, sedvh);
    std::cerr << "SimDepoSource got " << sedvh->size() << " depos from " << label << " bool return: " << okay << std::endl;

    if (!okay || sedvh->empty()) {
        std::string msg = "SimDepoSource failed to get sim::SimEnergyDeposit from label: " + label;
        std::cerr << msg << std::endl;
        THROW(WireCell::RuntimeError() << WireCell::errmsg{msg});
    }

    const size_t ndepos = sedvh->size();
    std::cerr << "SimDepoSource got " << ndepos << " depos\n";
    for (size_t ind=0; ind<ndepos; ++ind) {
        auto const& sed = sedvh->at(ind);
        auto pt = sed.MidPoint();
        const WireCell::Point wpt(pt.x(), pt.y(), pt.z());
        const double wt = sed.Time();
        const double wq = sed.Energy();
        // fixme: if (somecfg) { wq = sed.NumElectrons(); }
        const int wid = sed.TrackID();

        WireCell::IDepo::pointer depo
            = std::make_shared<WireCell::SimpleDepo>(wt, wpt, wq, nullptr, 0.0, 0.0, wid);
        m_depos.push_back(depo);
    }
    std::sort(m_depos.begin(), m_depos.end(), WireCell::ascending_time);
}

bool SimDepoSource::operator()(WireCell::IDepo::pointer& out)
{
    out = nullptr;
    if (m_depos.empty()) {
        return false;
    }

    out = m_depos.front();
    m_depos.pop_front();

    return true;
}
