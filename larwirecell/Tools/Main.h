#ifndef LARWIRECELL_TOOLS_MAIN
#define LARWIRECELL_TOOLS_MAIN

#include "larwirecell/Interfaces/MainTool.h"
#include "fhiclcpp/ParameterSet.h" 

namespace wcls {
    class Main : public MainTool {
    public:
        explicit Main(fhicl::ParameterSet const&);

        void process(art::Event& event);
    };
}

#endif
