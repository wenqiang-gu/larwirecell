// This is an example main Wire Cell Toolkit configuration file.
//
// NOTE: for production running, you don't want this file but rather a
// compiled compressed JSON file.  


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

// Note, don't forget to mention the wclsFrameSaver:nfsaver in the
// "outputers" list in the FHiCL file.
local nf_saver = {
    type: "wclsFrameSaver",
    name: "nfsaver",
    data: {
	anode: wc.tn(guts.anode),
	digitize: true,
	//pedestal_mean: "fiction",
	pedestal_mean: 0.0,
	pedestal_sigma: 1.75,
        frame_tags: ["raw"],
	nticks: params.detector.nticks,
	chanmaskmaps: ["bad"],
    }
};

// Note, don't forget to mention the wclsFrameSaver:spsaver in the
// "outputers" list in the FHiCL file.
local wcls_charge_scale = 200.0;
local sp_saver = {
    type: "wclsFrameSaver",
    name: "spsaver",
    data: {
	anode: wc.tn(guts.anode),
	digitize: false,
	sparse: true,
        frame_tags: ["gauss", "wiener"],
	frame_scale: wcls_charge_scale,
	nticks: params.detector.nticks,
	summary_tags: ["threshold"], 
	summary_scale: wcls_charge_scale,
    }
};

local filter_components = guts.noise_frame_filters + [nf_saver] + guts.sigproc_frame_filters + [sp_saver];


// now the main config sequence

[
    source,
    nf_saver,
    sp_saver,

] + guts.config_sequence + [

    {
        type: "Omnibus",
        data: {
            source: wc.tn(source),
            //sink: wc.tn(sink),
            filters: std.map(wc.tn, filter_components),
        }
    },
    
]    
