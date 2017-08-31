// This is a main Wire Cell Toolkit configuration file.
//
// It configures WCT to run inside LArSoft in order to run MicroBooNE
// noise filtering (NF) and signal processing (SP).

// Most of the configuration is provided as part of WCT as located by
// directories in a WIRECELL_PATH environment variable.

local wc = import "wirecell.jsonnet";
local params = import "params/chooser.jsonnet";


// replace the OmniChannelNoiseDB with a derived version that extends
// both the C++ and its configuration.
///// this is what's needed if using wclsChannelNoiseDB.
//
//local omnicndb_data = import "uboone/sigproc/omnicndb.jsonnet";
//local base_guts = import "uboone/sigproc/omni-nf-sp.jsonnet";
//local guts = base_guts {
//    noisedb: {
//	type: "wclsChannelNoiseDB",
//	data: omnicndb_data {
//	    bad_channel: { policy: "replace" },
//	    misconfig_channel: {
//                policy: "replace",
//                from: {gain:  4.7*wc.mV/wc.fC, shaping: 1.1*wc.us},
//                to:   {gain: 14.0*wc.mV/wc.fC, shaping: 2.2*wc.us},
//	    }
//	}
//    }
//};
///// else use statically configured.
local guts = import "uboone/sigproc/omni-nf-sp.jsonnet";


local source = {
    type: "wclsRawFrameSource",
    data: {
        source_label: "daq", 
        frame_tags: ["orig"],
	nticks: params.sigproc.frequency_bins,
    },
};
local sink = {
    type: "wclsCookedFrameSink",
    data: {
	anode: wc.tn(guts.anode),
        frame_tags: ["gauss", "wiener"],
	nticks: params.detector.nticks,
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
