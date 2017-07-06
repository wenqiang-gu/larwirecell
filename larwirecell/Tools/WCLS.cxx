#include "WCLS.h"

#include <vector>

using namespace wcls;

WCLS::WCLS(fhicl::ParameterSet const& ps)
    : m_wcmain()
{
    typedef std::vector<std::string> string_list_t;
    typedef std::pair<std::string,std::string> keyvalue_t;
    typedef std::vector<keyvalue_t> keyvalue_list_t;

    // transfer configuration
    if (ps.has_key("apps")) {
        auto const& apps = ps.get<string_list_t>("apps");
        for (auto app : apps) {
            m_wcmain.add_app(app);
        }
    }
    if (ps.has_key("configs")) {
        auto const& configs = ps.get<string_list_t>("configs");
        for (auto cfg : configs) {
            m_wcmain.add_config(cfg);
        }
    }
    if (ps.has_key("paths")) {
        auto const& paths = ps.get<string_list_t>("paths");
        for (auto path : paths) {
            m_wcmain.add_path(path);
        }
    }
    if (ps.has_key("plugins")) {
        auto const& plugins = ps.get<string_list_t>("plugins");
        for (auto plugin : plugins) {
            m_wcmain.add_plugin(plugin);
        }
    }

    if (ps.has_key("params")) {
        auto const& params = ps.get<keyvalue_list_t>("params");
        for (auto pv : params) {
            m_wcmain.add_var(pv.first, pv.second);
        }
    }

}
WCLS::~WCLS()
{
}

/// fixme: need some way to deliver services to components that want them.

void WCLS::initialize()
{
    m_wcmain.initialize();
}

void WCLS::process(art::Event& event)
{
    // fixme: need to iterate over all IArtEventVisitor and give them the event.


    m_wcmain();
}
