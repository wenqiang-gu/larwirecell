/** A sim depo source provides IDepo objects to WCT from LS simulation objects. */

#ifndef LARWIRECELL_COMPONENTS_SIMDEPOSOURCE
#define LARWIRECELL_COMPONENTS_SIMDEPOSOURCE

#include "larwirecell/Interfaces/IArtEventVisitor.h"
#include "WireCellIface/IDepoSource.h"
#include "WireCellIface/IConfigurable.h"
#include "WireCellIface/IDepo.h"

#include <deque>

namespace wcls {

    class SimDepoSource :  public IArtEventVisitor,
                           public WireCell::IDepoSource,
                           public WireCell::IConfigurable {
    public:
        SimDepoSource();
        virtual ~SimDepoSource();

        /// IArtEventVisitor 
        virtual void visit(art::Event & event);

        /// IDepoSource
        virtual bool operator()(WireCell::IDepo::pointer& out);

        /// IConfigurable
        virtual WireCell::Configuration default_configuration() const;
        virtual void configure(const WireCell::Configuration& config);

    private:
        std::deque<WireCell::IDepo::pointer> m_depos;
        bool m_eos;
    };
}
#endif
