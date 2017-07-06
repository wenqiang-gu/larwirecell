/** A wcls::MainTool is a main entry point to the WCT from LS */

// fixme: maybe need to pick a less generic name for this tool?

#ifndef LARWIRECELL_INTERFACES_MAINTOOL
#define LARWIRECELL_INTERFACES_MAINTOOL

namespace art {
    class Event;
}

namespace wcls {

    class MainTool {
    public:

        virtual ~MainTool() noexcept = default;
        virtual void initialize() = 0;
        virtual void process(art::Event& event) = 0;
    };
}

#endif
