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

// We need these in at least two places, so make temp/local vars.
local source = ls.source.frame.noisefiltered;
local sink = ls.sink.frame.cooked;

// Now comes the actual configuration sequence.
[
    source,

    anodes.nominal,

    bits.fieldresponse,

] + filters + [

    bits.perchanresp,

    // omni.noisefilter,
    omni.sigproc,
    
    sink,

    // The final app that handles data flow throught hte next level components
    {
        type: "Omnibus",
        data: {
            source: wc.tn(source),
            //filters: [wc.tn(omni.sigproc)],
            filters: [],        // empty for fast testing of output
            sink: wc.tn(sink),
        }
    },

]
