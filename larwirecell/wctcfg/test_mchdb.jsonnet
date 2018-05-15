// Test basics of configuring MultiChannelNoiseDB.
//
// WARNING: this is not meant to be configure a fully working job,
// don't use for anything real.

local wc = import "wirecell.jsonnet";

local anode = {
    type: "AnodePlane",
    data : {
        fields: "ub-10-half.json.bz2",
        wires: "microboone-celltree-wires-v2.json.bz2",
    }
};

local base_chndb_data = {
    nsamples: 10000,            // bogus
};

local chndb_prehwfix = {
    type: "wclsChannelNoiseDB",
    name: "chndbprehwfix",
    data: base_chndb_data {
        nsamples: 10001,            // bogus
    }
};


local chndb_posthwfix = {
    type: "wclsChannelNoiseDB",
    name: "chndbposthwfix",
    data: base_chndb_data {
        nsamples: 10002,            // bogus
    }
};

local chndb_catchall = {
    type: "OmniChannelNoiseDB",
    name: "chndbcatchall",
    data: base_chndb_data {
        nsamples: 10003,            // bogus
    }
};

local chndb_multi = {
    type: "wclsMultiChannelNoiseDB",
    data: {
        rules: [
            {
                rule: "runrange",
                chndb: wc.tn(chndb_prehwfix),
                args: {first: 4952, last:6998 }, // MB Run I, before hardware fix
            },
            {
                rule: "runstarting",
                chndb: wc.tn(chndb_posthwfix), // Likely start of post hardware fix.
                args: 7956,
            },
            {
                rule: "bool",
                chndb: wc.tn(chndb_catchall),
                args: true
            },
        ]
    }
};
local app = {
    type: "Pgrapher",
    data: { edges: [] }
};

[anode, chndb_prehwfix, chndb_posthwfix, chndb_catchall, chndb_multi, app]
