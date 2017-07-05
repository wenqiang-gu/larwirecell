#include "RawFrameSource.h"


#include "WireCellUtil/NamedFactory.h"

WIRECELL_FACTORY(wclsRawFrameSource, wcls::RawFrameSource, wcls::IArtEventVisitor, WireCell::IFrameSource)


using namespace wcls;
using namespace WireCell;

RawFrameSource::RawFrameSource()
    : m_event(nullptr)
{
}

RawFrameSource::~RawFrameSource()
{
}

void RawFrameSource::visit_art_event(art::Event & event)
{
    m_event = &event;           // must take care not to use this for long.
}

bool RawFrameSource::operator()(IFrame::pointer& frame)
{
    // set an IFrame based on last visited event.
    frame = nullptr;            // fixme
    // fixme: replace this with a make_shared<SimpleFrame>(sf); with
    // the "sf" filled with SimpleTrace objects that have been filled
    // with data from the m_event.

    return true;
}
