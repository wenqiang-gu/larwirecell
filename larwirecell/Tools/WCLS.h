#ifndef LARWIRECELL_TOOLS_WCLS
#define LARWIRECELL_TOOLS_WCLS


#include "WireCellApps/Main.h"
#include "larwirecell/Interfaces/MainTool.h"
#include "fhiclcpp/ParameterSet.h" 

#include <string>
#include <map>

namespace WireCell {
    class Main;
}

namespace wcls {

    // For now this just sits here until I can figure out how to
    // replicate for tool the features that modules support as
    // described here:
    // https://cdcvs.fnal.gov/redmine/projects/art/wiki/Configuration_validation_and_description
    // struct WCLSConfig {
    //     typedef fhicl::Sequence<std::string> string_list_t;
    //     typedef std::pair<std::string,std::string> keyvalue_t;
    //     typedef fhicl::Sequence<keyvalue_t> keyvalue_list_t;

    //     string_list_t apps { fhicl::Name("apps") };
    //     string_list_t configs { fhicl::Name("configs") };
    //     string_list_t paths { fhicl::Name("paths") };
    //     string_list_t plugins { fhicl::Name("plugin") };
    //     keyvalue_list_t params { fhicl::Name("params") };
    // };

    class WCLS : public MainTool {
    public:
        explicit WCLS(fhicl::ParameterSet const& ps);
        virtual ~WCLS();

        void initialize();
        void process(art::Event& event);

    private:
        WireCell::Main m_wcmain;
    };
}

#endif
