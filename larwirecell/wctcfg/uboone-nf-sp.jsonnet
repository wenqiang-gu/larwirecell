// This is a main WCT configuration file.
//
// It configures WCT to run inside LArSoft in order to run MicroBooNE
// noise filtering (NF) and signal processing (SP).

local wc = import "wirecell.jsonnet";
local guts = import "uboone/sigproc/omni-nf-sp.jsonnet";

local source = {
    type: "wclsRawFrameSource",
    data: {
        source_label: "daq", 
        frame_tags: ["orig"],
    },
};
local sink = {
    type: "wclsCookedFrameSink",
    data: {
	anode: wc.tn(guts.anode),
        frame_tags: ["gauss", "wiener"],
    }
};



// now the main config sequence

[
    source,
    sink,

] + guts.config_sequence + [

    {
        type: "Omnibus",
        data: {
            source: wc.tn(source),
            sink: wc.tn(sink),
            filters: std.map(wc.tn, guts.frame_filters)

        }
    },
    
]    
