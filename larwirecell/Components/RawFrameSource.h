/** A WCT component which is a source of raw frames which it produces
 * by also being an art::Event visitor.
 *
 * Raw means that the waveforms are taken from the art::Event as a
 * labeled std::vector<raw::RawDigit> collection.
 */

#ifndef LARWIRECELL_COMPONENTS_RAWFRAMESOURCE
#define LARWIRECELL_COMPONENTS_RAWFRAMESOURCE

#include "larwirecell/Interfaces/IArtEventVisitor.h"
#include "WireCellIface/IFrameSource.h"
#include "WireCellIface/IConfigurable.h"

#include <string>
#include <vector>

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
        std::string m_source;
        double m_tick;
	std::vector<std::string> m_frame_tags;
    };
}

#endif
