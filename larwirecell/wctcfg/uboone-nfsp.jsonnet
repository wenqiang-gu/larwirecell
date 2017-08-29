// This is a main Wire Cell Toolkit configuration file for use with LArSoft.
//
// It configures WCT to run inside LArSoft in order to run MicroBooNE
// noise filtering (NF) and signal processing (SP).

// Most of the configuration is provided as part of WCT as located by
// directories in a WIRECELL_PATH environment variable.


// These are provided by the wire-cell-cfg package.
local wc = import "wirecell.jsonnet";
local gen = import "uboone/nfsp/general.jsonnet";
local nf = import "uboone/nfsp/nf.jsonnet";
local sp = import "uboone/nfsp/sp.jsonnet";
local params = import "uboone/nfsp/params.jsonnet";
local chndb_data = import "uboone/nfsp/chndb_data.jsonnet"; 


// Override parts of the NF config in order to use LArSoft-specific
// channel noise database class.
local wcls_nf = nf {
    chndb : {
	type: "wclsChannelNoiseDB",
	data: chndb_data {
	    bad_channel: { policy: "replace" },
	    misconfig_channel: {
                policy: "replace",
                from: {gain:  4.7*wc.mV/wc.fC, shaping: 1.1*wc.us},
                to:   {gain: 14.0*wc.mV/wc.fC, shaping: 2.2*wc.us},
	    }
	}
    }
};

local source = {
    type: "wclsRawFrameSource",
    data: {
        source_label: "daq", 
        frame_tags: ["orig"],
	nticks: params.frequency_bins,
    },
};

local sink = {
    type: "wclsCookedFrameSink",
    data: {
        frame_tags: ["gauss", "wiener"],
	nticks: params.output_nticks,
    },
};


// now the main config sequence

[
    source,
    sink,

] + gen.sequence + wcls_nf.sequence + sp.sequence + [

    {
        type: "Omnibus",
        data: {
            source: wc.tn(source),
            sink: wc.tn(sink),
            filters: std.map(wc.tn, wcls_nf.frame_filters + sp.frame_filters)
        }
    },
    
]    
