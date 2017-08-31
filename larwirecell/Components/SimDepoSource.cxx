#include "SimDepoSource.h"

#include "WireCellUtil/NamedFactory.h"

WIRECELL_FACTORY(wclsSimDepoSource, wcls::SimDepoSource, wcls::IArtEventVisitor, WireCell::IDepoSource)


using namespace wcls;
using namespace WireCell;

SimDepoSource::SimDepoSource()
    : m_event(nullptr)
{
}

SimDepoSource::~SimDepoSource()
{
}

void SimDepoSource::visit(art::Event & event)
{
    m_event = &event;           // must take care not to use this for long.
}

bool SimDepoSource::operator()(IDepo::pointer& out)
{
    // Set the next deposition.  Maintain strict time order!
    out = nullptr;            // fixme

    return true;
}
