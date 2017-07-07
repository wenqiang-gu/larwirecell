#include "art/Utilities/ToolMacros.h"
#include "art/Utilities/ToolConfigTable.h"

#include "larwirecell/Interfaces/MainTool.h"
#include "larwirecell/Interfaces/IArtEventVisitor.h"

#include "fhiclcpp/ParameterSet.h" 
#include "fhiclcpp/types/Sequence.h" 
#include "fhiclcpp/types/OptionalSequence.h" 
#include "fhiclcpp/types/OptionalTable.h" 
#include "fhiclcpp/types/Comment.h" 
#include "fhiclcpp/types/Table.h" 

#include "WireCellApps/Main.h"
#include "WireCellUtil/NamedFactory.h"

#include <vector>
#include <string>
#include <map>


namespace wcls {


    // https://cdcvs.fnal.gov/redmine/projects/fhicl-cpp/wiki/Fhiclcpp_types_in_detail#TableltT-KeysToIgnoregt
    struct WCLSKeysToIgnore {
        std::set<std::string> operator()() {
            // Ignore these for validation.
            return {"params"};
        }
    };

    // https://cdcvs.fnal.gov/redmine/projects/art/wiki/Configuration_validation_and_description
    struct WCLSConfig {
        typedef fhicl::Sequence<std::string> string_list_t;
        typedef fhicl::OptionalSequence<std::string> optional_string_list_t;
        typedef fhicl::OptionalTable<fhicl::ParameterSet> optional_pset_t;

        // These are items needed by the tool
        optional_string_list_t inputers { fhicl::Name("inputers"),
                fhicl::Comment("List of input converters.") };
        optional_string_list_t outputers { fhicl::Name("outputers"),
                fhicl::Comment("List of ouput converters.") };

        // These are WCT config items to pass through
        string_list_t configs { fhicl::Name("configs"),
                fhicl::Comment("List of WCT configuration files.") };
        optional_string_list_t apps { fhicl::Name("apps"),
                fhicl::Comment("List of WCT application objects to execute.")};
        optional_string_list_t paths { fhicl::Name("paths"),
                fhicl::Comment("File system paths in which to locate WCT files.")};
        optional_string_list_t plugins { fhicl::Name("plugins"),
                fhicl::Comment("List of WCT component plugins to load.")};
        optional_pset_t params{ fhicl::Name("params"),
                fhicl::Comment("External variables to inject into WCT configuration.")};
    };

    class WCLS : public MainTool {
    public:
        using Parameters = art::ToolConfigTable<WCLSConfig, WCLSKeysToIgnore>;

        explicit WCLS(Parameters const& ps);
        virtual ~WCLS() { }

        void process(art::Event& event);

    private:
        WireCell::Main m_wcmain;
        wcls::IArtEventVisitor::vector m_inputers, m_outputers;
    };
}



wcls::WCLS::WCLS(wcls::WCLS::Parameters const& params)
    : m_wcmain()
{
    const auto& wclscfg = params();

    // transfer configuration

    // required
    for (auto cfg : wclscfg.configs()) {
        m_wcmain.add_config(cfg);
    }
    

    WCLSConfig::optional_string_list_t::rtype slist;
    if (wclscfg.apps(slist)) {
        for (auto app : slist) {
            m_wcmain.add_app(app);
        }
    }
    slist.clear();
    if (wclscfg.paths(slist)) {
        for (auto path : slist) {
            m_wcmain.add_path(path);
        }
    }
    slist.clear();
    if (wclscfg.plugins(slist)) {
        for (auto plugin : slist) {
            m_wcmain.add_plugin(plugin);
        }
    }
    slist.clear();

    fhicl::ParameterSet wcpars;
    if (wclscfg.params(wcpars)) {
        for (auto key : wcpars.get_names()) {
            auto value = wcpars.get<std::string>(key);
            m_wcmain.add_var(key, value);
        }
    }


    m_wcmain.initialize();


    if (wclscfg.inputers(slist)) {
        for (auto inputer : slist) {
            auto iaev = WireCell::Factory::find_tn<IArtEventVisitor>(inputer);
            m_inputers.push_back(iaev);
        }
    }
    slist.clear();
    if (wclscfg.outputers(slist)) {
        for (auto outputer : slist) {
            auto iaev = WireCell::Factory::find_tn<IArtEventVisitor>(outputer);
            m_outputers.push_back(iaev);
        }
    }
    slist.clear();
}

void wcls::WCLS::process(art::Event& event) {
    for (auto iaev : m_inputers) {
        iaev->visit(event);
    }
    
    m_wcmain();
    
    for (auto iaev : m_outputers) {
        iaev->visit(event);
    }
}


DEFINE_ART_CLASS_TOOL(wcls::WCLS)
