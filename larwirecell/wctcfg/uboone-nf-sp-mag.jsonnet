// This is a main WCT configuration file.
//
// It configures WCT to run inside LArSoft in order to run MicroBooNE
// noise filtering (NF) and signal processing (SP).

local wc = import "wirecell.jsonnet";
local guts = import "uboone/sigproc/omni-nf-sp.jsonnet";
local magnify = import "uboone/io/magnify.jsonnet";

local source = {
    type: "wclsRawFrameSource",
    data: {
        source_label: "daq", 
        frame_tags: ["orig"],
    },
};

local in_sink = magnify.sink {
    name: "in_sink",
    data: super.data {
	frames: ["orig"],
	root_file_mode: "RECREATE",
    },
};

local nf_sink = magnify.sink {
    name: "nf_sink",
    data: super.data {
	frames: ["raw"],
	root_file_mode: "UPDATE",
    },
};

local sp_sink = magnify.sink {
    name: "sp_sink",
    data: super.data {
        frames: ["wiener", "gauss"],
	root_file_mode: "UPDATE",
        cmmtree: [["bad","T_bad"], ["lf_noisy", "T_lf"]],
        summaries: ["threshold"],
    }
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
    in_sink,
    nf_sink,
    sp_sink,
    sink,

] + guts.config_sequence + [

    {
        type: "Omnibus",
        data: {
            source: wc.tn(source),
            sink: wc.tn(sink),
            filters: [		// linear chaining of frame filters
		wc.tn(in_sink)
	    ] + std.map(wc.tn, guts.noise_frame_filters) + [
		wc.tn(nf_sink),
	    ] + std.map(wc.tn, guts.sigproc_frame_filters) + [
		wc.tn(sp_sink),
            ],
        }
    },
    
]    
