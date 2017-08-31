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

local magnify_sink = {
    type: "MagnifySink",
    data: {
        input_filename: "/dev/null",
        output_filename: std.extVar("output"),
	anode: wc.tn(guts.anode),
	runinfo: null,
	root_file_mode: "UPDATE",
        frames: [],
        summaries: [],
        shunt: [],
        cmmtree: [ ],
    }
};

local in_sink = magnify_sink {
    name: "in_sink",
    data: super.data {
	frames: ["orig"],
	root_file_mode: "RECREATE",
	runinfo : {		// fill in bogus Trun
	    detector: 0,
	    eventNo: 0,
	    runNo: 0,
	    subRunNo: 0,
	    unit_dis: 1.116,
	    toffset_uv: 0.0,
	    toffset_uw: 0.0,
	    toffset_u: 0.0,
	    total_time_bin: 0,
	    frame_length: 0,
	    eve_num: 0,
	    nrebin: 1,
	    time_offset: 0,
	}
    },
};

local nf_sink = magnify_sink {
    name: "nf_sink",
    data: super.data {
	frames: ["raw"],
    },
};

local sp_sink = magnify_sink {
    name: "sp_sink",
    data: super.data {
        frames: ["wiener", "gauss"],
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
