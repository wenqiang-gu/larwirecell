#include "art/Utilities/ToolMacros.h"
#include "larwirecell/Interfaces/IArtEventVisitor.h"

namespace wcls {
    class Main : public IArtEventVisitor {
    public:
        explicit Main(fhicl::ParameterSet const&) {
            // fixme: unpack, convert and feed config to WCT's main.
        }
        virtual ~Main() { }

        void visit_art_event(art::Event& event) {
            // fixme: find all things that want an event and give it to them, then run WCT main.
        }
    };
}

DEFINE_ART_CLASS_TOOL(wcls::Main)
