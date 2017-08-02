// This is a main WCT configuration file.
//
// It configures WCT so simply pass through a frame.

local wc = import "wirecell.jsonnet";
local anodes = import "multi/anodes.jsonnet"; 

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
	anode: wc.tn(anodes.nominal),
        frame_tags: ["orig"],
    }
};



// now the main config sequence

[
    source,
    sink,
    {
        type: "Omnibus",
        data: {
            source: wc.tn(source),
            sink: wc.tn(sink),
            filters: [],
        }
    },
    
]    
