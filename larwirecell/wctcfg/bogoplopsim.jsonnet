// This is a WCT configuration file for use in a little test WC/LS
// job.  It is expected to be named inside a FHiCL configuration of
// the same name as this one but with .fcl extension.


local wc = import "wirecell.jsonnet";
local g = import "pgraph.jsonnet";


local params = import "pgrapher/experiment/uboone/simparams.jsonnet";
local tools_maker = import "pgrapher/common/tools.jsonnet";
local tools = tools_maker(params);
local anode = tools.anodes[0];

local sim_maker = import "pgrapher/experiment/uboone/sim.jsonnet";
local sim = sim_maker(params, tools);


local nf_maker = import "pgrapher/experiment/uboone/nf.jsonnet";
local chndb_maker = import "pgrapher/experiment/uboone/chndb.jsonnet";

local sp_maker = import "pgrapher/experiment/uboone/sp.jsonnet";

    
// This gets used as an art::Event tag and the final output frame
// needs to be tagged likewise.  Note, art does not allow some
// characters, in particular: [_-]
local digit_tag = "wctdigits";

// make sure name matches calling FHiCL
local depos = g.pnode({
    type: 'wclsSimDepoSource',
    name: "",
    data: {
        model: "",              // empty means art depos hold #electrons
        scale: 1.0,

        // these must be coordinated with the FHiCL
        art_label: "plopper",   // name of upstream art producer of the depos
        art_instance: "bogus",  // name given to the object instance of the depo collection
    },
}, nin=0, nout=1);

local frames = g.pnode({
    type: "wclsFrameSaver",
    name: "", 
    data: {
        anode: wc.tn(anode),
        digitize: true,         // true means save as RawDigit, else recob::Wire
        frame_tags: [digit_tag],
        nticks: params.daq.nticks,
        chanmaskmaps: [],
    },
}, nin=1, nout=1, uses=[anode]);


local drifter = sim.drifter;

// Signal simulation.
local ductors = sim.make_anode_ductors(anode);
local md_pipes = sim.multi_ductor_pipes(ductors);
local ductor = sim.multi_ductor_graph(anode, md_pipes, "mdg");

// misconfigure some signal channels
local miscon = sim.misconfigure(params);

// Noise simulation adds to signal.
local noise_model = sim.make_noise_model(anode, sim.empty_csdb);
local noise = sim.add_noise(noise_model);

local digitizer = sim.digitizer(anode, tag=digit_tag);

//  Set the noise expoch to before or after the hw fix or "perfect"
local noise_epoch = "after";
local chndb = chndb_maker(params, tools).wct(noise_epoch);

// this just caps off the graph.
local sink = sim.frame_sink;


local graph = g.pipeline([depos, // input
                          drifter, ductor, miscon, noise, digitizer, // sim
                          frames, // output
                          sink]);


local app = {
    type: "Pgrapher",
    data: {
        edges: graph.edges,
    },
};

// Finally, the configuration sequence which is emitted.

graph.uses + [app]
