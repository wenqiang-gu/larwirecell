/** A wcls::MainTool is a main entry point to the WCT from LS */

#ifndef LARWIRECELL_INTERFACES_MAINTOOL
#define LARWIRECELL_INTERFACES_MAINTOOL

namespace art {
    class Event;
}

namespace wcls {

    class MainTool {
    public:
        virtual ~MainTool() noexcept = default;
        virtual void process(art::Event& event) = 0;
    };
}

#endif
