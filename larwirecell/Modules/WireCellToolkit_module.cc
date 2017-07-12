#include "art/Framework/Core/ModuleMacros.h" 
#include "art/Framework/Core/EDProducer.h"
#include "art/Utilities/make_tool.h" 
#include "larwirecell/Interfaces/MainTool.h"

// fixme: this should be removed once produces<>() is moved to sink components.
#include "lardataobj/RecoBase/Wire.h"

#include <iostream>

using namespace std;

namespace wcls {

    class WireCellToolkit : public art::EDProducer {
    public:
        explicit WireCellToolkit(fhicl::ParameterSet const& pset);
        virtual ~WireCellToolkit();

        void produce(art::Event & evt);
        void reconfigure(fhicl::ParameterSet const& pset);
    
        void beginJob();
        void endJob();

    private:

        std::unique_ptr<wcls::MainTool> m_wcls;
        
    };
}


wcls::WireCellToolkit::WireCellToolkit(fhicl::ParameterSet const& pset)
    : EDProducer()
{
    cerr << "WireCellToolkit constructed at " << (void*)this << endl;
    this->reconfigure(pset);

    // fixme: this needs to be moved into the sink components somehow.
    //EDProducer* prod = this;
    //prod->produces< std::vector<recob::Wire> >();
}
wcls::WireCellToolkit::~WireCellToolkit()
{
    cerr << "WireCellToolkit destructed\n";
}

void wcls::WireCellToolkit::produce(art::Event & evt)
{
    cerr << "WireCellToolkit produce\n";
    m_wcls->process(evt);
}

void wcls::WireCellToolkit::reconfigure(fhicl::ParameterSet const& pset)
{
    cerr << "WireCellToolkit reconfigure\n";
    auto const& wclsPS = pset.get<fhicl::ParameterSet>("wcls_main");
    m_wcls = art::make_tool<wcls::MainTool>(wclsPS);
    if (! m_wcls) {
        throw cet::exception("WireCellToolkit_module") << "Failed to get Art Tool \"wcls_main\"\n";
    }
    m_wcls->produces(this);
}
    
void wcls::WireCellToolkit::beginJob()
{
    cerr << "WireCellToolkit begin job\n";
}

void wcls::WireCellToolkit::endJob()
{
    cerr << "WireCellToolkit end job\n";
}

namespace wcls{
    DEFINE_ART_MODULE(WireCellToolkit)
}
