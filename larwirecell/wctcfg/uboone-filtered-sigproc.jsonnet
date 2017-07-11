// This is an example of a main WCT config file for microboone signal
// processing.  The Jsonnet it contains compiles to a JSON array of
// JSON objects, one for each WCT component which are composed
// starting in the WCT app components.
local wc = import "wirecell.jsonnet";
local ls = import "larsoft.jsonnet";

local anodes = import "multi/anodes.jsonnet";
local bits = import "uboone/sigproc/bits.jsonnet";
local filters = import "uboone/sigproc/filters.jsonnet";
local omni = import "uboone/sigproc/omni.jsonnet";

local source = ls.source.frame.noisefiltered;
[

    source,

    anodes.nominal,

    bits.fieldresponse,

] + filters + [

    bits.perchanresp,

    // omni.noisefilter,
    omni.sigproc,
    
    // fixme: need a sink
    {
        type: "Omnibus",
        data: {
            source: wc.tn(source),
            sink: "",
            filters: [wc.tn(omni.sigproc)],
        }
    },


]
