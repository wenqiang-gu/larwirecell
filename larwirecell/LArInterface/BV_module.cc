#include "art/Framework/Core/ModuleMacros.h" 
#include "art/Framework/Core/EDProducer.h"
#include "art/Utilities/make_tool.h" 
#include "larwirecell/Interfaces/MainTool.h"

#include <iostream>

using namespace std;

namespace wct {

    class BV : public art::EDProducer {
    public:
        explicit BV(fhicl::ParameterSet const& pset);
        virtual ~BV();

        void produce(art::Event & evt);
        void reconfigure(fhicl::ParameterSet const& pset);
    
        void beginJob();
        void endJob();

    private:

        std::unique_ptr<wcls::MainTool> m_wcls;
        
    };
}


wct::BV::BV(fhicl::ParameterSet const& pset)
    : EDProducer()
{
    cerr << "BV constructed\n";
    this->reconfigure(pset);
    //produces<int>();
}
wct::BV::~BV()
{
    cerr << "BV destructed\n";
}

void wct::BV::produce(art::Event & evt)
{
    cerr << "BV produce\n";
    m_wcls->process(evt);
}

void wct::BV::reconfigure(fhicl::ParameterSet const& pset)
{
    cerr << "BV reconfigure\n";
    auto const& wclsPS = pset.get<fhicl::ParameterSet>("wcls_main");
    m_wcls = art::make_tool<wcls::MainTool>(wclsPS);
}
    
void wct::BV::beginJob()
{
    cerr << "BV begin job\n";
}

void wct::BV::endJob()
{
    cerr << "BV end job\n";
}

namespace wct{
    DEFINE_ART_MODULE(BV)
}
