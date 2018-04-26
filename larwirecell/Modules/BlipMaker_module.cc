#include "art/Framework/Core/ModuleMacros.h" 
#include "art/Framework/Core/EDProducer.h"

#include "lardataobj/Simulation/SimEnergyDeposit.h"

namespace bogoblip {
    struct BlipMakerConfig {
    };
    class BlipMaker : public art::EDProducer {
    public:

	using Parameters = art::EDProducer::Table<BlipMakerConfig>;

        explicit BlipMaker(Parameters const& params);
        virtual ~BlipMaker();

        void produce(art::Event & evt);
    
    private:

	const BlipMakerConfig m_cfg;
        int m_count;
    };
}

#include <string>
const std::string label = "bogus"; // fixme: make configurable

bogoblip::BlipMaker::BlipMaker(Parameters const& params)
    : m_cfg(params())
    , m_count(0)
{
    produces< std::vector<sim::SimEnergyDeposit> >(label);
}
bogoblip::BlipMaker::~BlipMaker()
{
}
void bogoblip::BlipMaker::produce(art::Event & event)
{
    ++m_count;
    const double jump = m_count*1e9; // each call jump forward in time.

    auto out = std::make_unique< std::vector<sim::SimEnergyDeposit> >();
    
    const int nphotons = 0;
    const int nelepercm = 50000;
    const double mevpercm = 2.0;

    const double t0 = 0.;
    const double t1 = 0.;
    const int trackid = 0;

    
    // implicit units are cm, ns and MeV.
    const sim::SimEnergyDeposit::Point_t start = {100.,0.,0.};
    const sim::SimEnergyDeposit::Point_t end = {150.,10.,50.};
    const auto vdiff = end-start;
    const auto vlen = sqrt(vdiff.Mag2());
    const auto vdir = vdiff.unit();

    const double stepsize = 0.1; // cm
    const int nsteps = vlen/stepsize;

    sim::SimEnergyDeposit::Point_t last = start;
    for (int istep=1; istep<nsteps; ++istep) {
        const sim::SimEnergyDeposit::Point_t next(start + stepsize*istep*vdir);
        //std::cerr << last << " -> " << next << "\n";
        out->emplace_back(sim::SimEnergyDeposit(nphotons,
                                                stepsize * nelepercm,
                                                stepsize * mevpercm,
                                                last, next,
                                                jump + t0,
                                                jump + t1, trackid));
        last = next;
    }

    std::cerr << "BlipMaker making " << out->size() << " depos to label: " << label << std::endl;


    event.put(std::move(out), label);
}


namespace bogoblip {
    DEFINE_ART_MODULE(BlipMaker)
}

