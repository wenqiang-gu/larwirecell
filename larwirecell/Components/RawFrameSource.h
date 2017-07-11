/** A wct/ls raw frame source and art::Event visitor. 
 *
 * This is a source from the point of view of WCT.  It's conceptually
 * a sink from the point of view of LArsoft.
 */

#ifndef LARWIRECELL_COMPONENTS_RAWFRAMESOURCE
#define LARWIRECELL_COMPONENTS_RAWFRAMESOURCE

#include "larwirecell/Interfaces/IArtEventVisitor.h"
#include "WireCellIface/IFrameSource.h"
#include "WireCellIface/IConfigurable.h"

namespace wcls {
    class RawFrameSource : public IArtEventVisitor, 
                           public WireCell::IFrameSource,
                           public WireCell::IConfigurable { 
    public:
        RawFrameSource();
        virtual ~RawFrameSource();

        /// IArtEventVisitor 
        virtual void visit(art::Event & event);

        /// IFrameSource
        virtual bool operator()(WireCell::IFrame::pointer& frame);            

        /// IConfigurable
        virtual WireCell::Configuration default_configuration() const;
        virtual void configure(const WireCell::Configuration& config);

    private:
        WireCell::IFrame::pointer m_frame;
        WireCell::Configuration m_cfg;
    };
}

#endif
