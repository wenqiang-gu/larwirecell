/** A WCT component which passes through IFrames untouched and saves
 * their content as:

 - waveform content as vector collections of either raw::RawDigit or
   recob::Wire,

 - summaries as vector<double>

 - channel mask maps as vector<int> holding channel numbers

It can be configured to scale waveform or summary values by some constant.
*/

#ifndef LARWIRECELL_COMPONENTS_FRAMESAVER
#define LARWIRECELL_COMPONENTS_FRAMESAVER

#include "WireCellIface/IFrameFilter.h"
#include "WireCellIface/IConfigurable.h"
#include "WireCellIface/IAnodePlane.h"
#include "larwirecell/Interfaces/IArtEventVisitor.h"

#include <string>
#include <vector>

namespace wcls {
    class FrameSaver : public IArtEventVisitor, 
                            public WireCell::IFrameFilter,
                            public WireCell::IConfigurable {
    public:
        FrameSaver();
        virtual ~FrameSaver();

        /// IArtEventVisitor 
        virtual void produces(art::EDProducer* prod);
        virtual void visit(art::Event & event);

        /// IFrameFilter
        virtual bool operator()(const WireCell::IFrame::pointer& inframe,
				WireCell::IFrame::pointer& outframe);

        /// IConfigurable
        virtual WireCell::Configuration default_configuration() const;
        virtual void configure(const WireCell::Configuration& config);

    private:
        WireCell::IFrame::pointer m_frame;
        WireCell::IAnodePlane::pointer m_anode;
	std::vector<std::string> m_frame_tags, m_summary_tags;
	std::vector<double> m_frame_scale, m_summary_scale;
	int m_nticks;
	bool m_digitize, m_sparse;
	Json::Value m_cmms, m_pedestal_mean;
	double m_pedestal_sigma;

	void save_as_raw(art::Event & event);
	void save_as_cooked(art::Event & event);
	void save_summaries(art::Event & event);
	void save_cmms(art::Event & event);

    };
}

#endif
