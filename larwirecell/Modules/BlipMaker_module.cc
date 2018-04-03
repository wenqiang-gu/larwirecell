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
    };
}

#include <string>
const std::string label = "bogus"; // fixme: make configurable

bogoblip::BlipMaker::BlipMaker(Parameters const& params)
    : m_cfg(params())
{
    produces< std::vector<sim::SimEnergyDeposit> >(label);
}
bogoblip::BlipMaker::~BlipMaker()
{
}
void bogoblip::BlipMaker::produce(art::Event & event)
{

    auto out = std::make_unique< std::vector<sim::SimEnergyDeposit> >();
    
    const int nphotons = 0;
    const int nelepercm = 50000;
    const double mevpercm = 2.0;

    const double t0 = 0.;
    const double t1 = 0.;
    const int trackid = 0;

    

    const sim::SimEnergyDeposit::Point_t start = {100.,0.,0.};
    const sim::SimEnergyDeposit::Point_t end = {110.,0.,100.};
    auto vdiff = end-start;
    auto vlen = sqrt(vdiff.Mag2());
    auto vdir = vdiff.unit();

    const double stepsize = 0.1; // cm
    const int nsteps = vlen/stepsize;

    sim::SimEnergyDeposit::Point_t last = start;
    for (int istep=1; istep<nsteps; ++istep) {
        const sim::SimEnergyDeposit::Point_t next(stepsize*istep*vdir);
        out->emplace_back(sim::SimEnergyDeposit(nphotons,
                                                stepsize * nelepercm,
                                                stepsize * mevpercm,
                                                last, next,
                                                t0, t1, trackid));
        last = next;
    }

    std::cerr << "BlipMaker making " << out->size() << " depos to label: " << label << std::endl;


    event.put(std::move(out), label);
}


namespace bogoblip {
    DEFINE_ART_MODULE(BlipMaker)
}

