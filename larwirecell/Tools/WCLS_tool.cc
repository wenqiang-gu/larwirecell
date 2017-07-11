#include "art/Utilities/ToolMacros.h"
#include "art/Utilities/ToolConfigTable.h"

#include "larwirecell/Interfaces/MainTool.h"
#include "larwirecell/Interfaces/IArtEventVisitor.h"

#include "fhiclcpp/ParameterSet.h" 
#include "fhiclcpp/types/Sequence.h" 
#include "fhiclcpp/types/OptionalSequence.h" 
#include "fhiclcpp/types/OptionalTable.h" 
#include "fhiclcpp/types/DelegatedParameter.h" 
#include "fhiclcpp/types/OptionalDelegatedParameter.h" 
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
        //typedef fhicl::OptionalTable<fhicl::ParameterSet> generic_pset_t;
        typedef fhicl::OptionalDelegatedParameter generic_pset_t;

        // These are WCT config items to pass through
        string_list_t configs { fhicl::Name("configs"),
                fhicl::Comment("List of one or more WCT configuration files."
                               "\nThey are located w.r.t. the WCT load path.") };
        string_list_t apps { fhicl::Name("apps"),
                fhicl::Comment("List of one or more WCT application objects to execute.")};
        string_list_t plugins { fhicl::Name("plugins"),
                fhicl::Comment("List of WCT component plugin libraries to load.\n"
                               "They are located w.r.t. the OS library load path")};

        optional_string_list_t paths { fhicl::Name("paths"),
                fhicl::Comment("Optional list of file system paths to add to the WCT "
                               "configuration file load path."
                               "\nThis augments the WIRECELL_PATH environment variable.")};
        generic_pset_t params{ fhicl::Name("params"),
                fhicl::Comment("Optional table giving external variables to inject into WCT configuration.")};

        // These are items needed by the tool
        optional_string_list_t inputers { fhicl::Name("inputers"),
                fhicl::Comment("List of WCT components which act as WCT sources.\n"
                               "They are called before WCT executes on each Art Event object") };
        optional_string_list_t outputers { fhicl::Name("outputers"),
                fhicl::Comment("List of WCT components which act as WCT sinks.\n"
                               "They are called after WCT executes on each Art Event object.") };
        
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
    
    for (auto app : wclscfg.apps()) {
        m_wcmain.add_app(app);
    }

    for (auto plugin : wclscfg.plugins()) {
        m_wcmain.add_plugin(plugin);
    }

    // optional

    WCLSConfig::optional_string_list_t::rtype slist;
    if (wclscfg.paths(slist)) {
        for (auto path : slist) {
            m_wcmain.add_path(path);
        }
    }
    slist.clear();


    {
        fhicl::ParameterSet wcpars;
        if (wclscfg.params.get_if_present(wcpars)) {
            for (auto key : wcpars.get_names()) {
                auto value = wcpars.get<std::string>(key);
                m_wcmain.add_var(key, value);
            }
        }
    }

    std::cerr << "Initialize Wire Cell\n";
    try {
        m_wcmain.initialize();
    }
    catch (WireCell::Exception& e) {
        std::cerr << "Wire Cell Toolkit threw an exception\n";
        auto msg = errstr(e);
        std::cerr << msg << std::endl;
        throw cet::exception("WireCellLArSoft") << msg;
    }



    if (wclscfg.inputers(slist)) {
        for (auto inputer : slist) {
            auto iaev = WireCell::Factory::find_tn<IArtEventVisitor>(inputer);
            m_inputers.push_back(iaev);
            std::cerr << "Inputer: \"" << inputer << "\"\n";
        }
    }
    slist.clear();
    if (wclscfg.outputers(slist)) {
        for (auto outputer : slist) {
            auto iaev = WireCell::Factory::find_tn<IArtEventVisitor>(outputer);
            m_outputers.push_back(iaev);
            std::cerr << "Outputer: \"" << outputer << "\"\n";
        }
    }
    slist.clear();
}

void wcls::WCLS::process(art::Event& event)
{
    for (auto iaev : m_inputers) {
        std::cerr << "pre visit\n";
        iaev->visit(event);
    }
    
    std::cerr << "Running Wire Cell Toolkit...\n";
    m_wcmain();
    std::cerr << "... Wire Cell Toolkit done\n";
    
    for (auto iaev : m_outputers) {
        std::cerr << "post visit\n";
        iaev->visit(event);
    }
}


DEFINE_ART_CLASS_TOOL(wcls::WCLS)
