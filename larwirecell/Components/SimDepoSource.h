/** A sim depo source provides IDepo objects to WCT from LS simulation objects. */

#ifndef LARWIRECELL_COMPONENTS_SIMDEPOSOURCE
#define LARWIRECELL_COMPONENTS_SIMDEPOSOURCE

#include "larwirecell/Interfaces/IArtEventVisitor.h"
#include "WireCellIface/IDepoSource.h"

namespace wcls {

    class SimDepoSource :  public IArtEventVisitor, // likely needs to be a configurable too.
                           public WireCell::IDepoSource { 
    public:
        SimDepoSource();
        virtual ~SimDepoSource();

        /// IArtEventVisitor 
        virtual void visit(art::Event & event);

        /// IDepoSource
        virtual bool operator()(WireCell::IDepo::pointer& out);

    private:
        art::Event* m_event;    // fixme: may not be best to hang on to this

    };
}
#endif
