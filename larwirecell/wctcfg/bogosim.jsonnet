// This is a simple Wire Cell Toolkit configuration file to excercise
// the test for simulation integration.  It's likely specified in side
// of bogoplopsim.fcl.  The assumption is that a vector of
// sim::SimEnergyDeposit can be found by the wclsSimDepoSource in the
// art::Event, they are fed into WCT simulation and the resulting WCT
// frames are written into art::Event as a vector of raw::RawDigit by
// the wclsFrameSaver.
//
// This is configured to use WCT's "MultiDuctor" which allows
// interleaving different field responses in order to simulate shorted
// wires.  It also provides a noise simulation.


local wc = import "wirecell.jsonnet";
local v = import "vector.jsonnet";

local base_params = {
    local par = self,           // make available to inner data structures

    lar : {
        DL :  7.2 * wc.cm2/wc.s,
        DT : 12.0 * wc.cm2/wc.s,
        lifetime : 8*wc.ms,
        drift_speed : 1.6*wc.mm/wc.us, // 500 V/cm
        density: 1.389*wc.g/wc.centimeter3,
        ar39activity: 1*wc.Bq/wc.kg,
    },
    detector : {
        // Relative extent for active region of LAr box.  
        // (x,y,z) = (drift distance, active height, active width)
        extent: [1*wc.m,1*wc.m,1*wc.m],
        // the center MUST be expressed in the same coordinate system
        // as the wire endpoints given in the files.wires entry below.
        // Default here is that the extent is centered on the origin
        // of the wire coordinate system.
        center: [0,0,0],
        drift_time: self.extent[0]/par.lar.drift_speed,
        drift_volume: self.extent[0]*self.extent[1]*self.extent[2],
        drift_mass: par.lar.density * self.drift_volume,
    },
    daq : {
        readout_time: 5*wc.ms,
        nreadouts: 1,
        start_time: 0.0*wc.s,
        stop_time: self.start_time + self.nreadouts*self.readout_time,
        tick: 0.5*wc.us,        // digitization time period
        sample_period: 0.5*wc.us, // sample time for any spectral data - usually same as tick
        first_frame_number: 100,
        ticks_per_readout: self.readout_time/self.tick,
    },
    adc : {
        gain: 1.0,
        baselines: [900*wc.millivolt,900*wc.millivolt,200*wc.millivolt],
        resolution: 12,
        fullscale: [0*wc.volt, 2.0*wc.volt],
    },
    elec : {
        gain : 14.0*wc.mV/wc.fC,
        shaping : 2.0*wc.us,
        postgain: -1.2,
    },
    sim : {
        fluctuate: true,
        digitize: true,
        noise: false,

        // continuous makes the WCT sim act like the streaming
        // detector+daq.  This means producing readout even if there
        // may be no depos.  If true then readout is based on chunking
        // up depos in time and there may be periods of time that do
        // not have any readouts.
        continuous: false,
    },
    files : {                   // each detector MUST fill these in.
        wires: null,
        fields: null,
        noise: null,
    }
};

local uboone_params = base_params {
    lar : super.lar {
        drift_speed : 1.114*wc.mm/wc.us, // at microboone voltage
    },
    detector : super.detector {
        // these two vectors define the origin of a Cartesian
        // coordinate system.  First sets the size of a box which is
        // the sensitive volume:
        extent: [2.5604*wc.m,2.325*wc.m,10.368*wc.m],
        // Next says where the center of that box is expressed
        // relative to whatever the origin is.  For MB we want this
        // box placed so that it has one plane at X=0 and another a
        // Z=0 and then centered on Y=0.
        center: [0.5*self.extent[0], 0.0, 0.5*self.extent[2]],
    },
    elec : super.elec {
        postgain: -1.2,
    },
    files : {
        wires:"microboone-celltree-wires-v2.1.json.bz2",
        fields:"ub-10-half.json.bz2",
        noise: "microboone-noise-spectra-v2.json.bz2",
    }
};
local params = uboone_params;



local random = {
    type: "Random",
    data: {
        generator: "default",
        seeds: [0,1,2,3,4],
    }
};


local source = {
    type: "wclsSimDepoSource",
};


local anode_nominal = {
    type : "AnodePlane",
    name : "nominal",
    data : params.elec + params.daq + params.files {
        ident : 0,
    }
};
local anode_uvground = anode_nominal {
    name: "uvground",
    data : super.data {
        fields: "ub-10-uv-ground-half.json.bz2",
    }
};
local anode_vyground = anode_nominal {
    name: "vyground",
    data : super.data {
        fields: "ub-10-vy-ground-half.json.bz2",
    }
};
local anodes = [anode_nominal, anode_vyground, anode_uvground];

local noise_model = {
    type: "EmpiricalNoiseModel",
    data: {
        anode: wc.tn(anode_nominal),
        spectra_file: params.files.noise,
        chanstat: "StaticChannelStatus",
        nsamples: params.daq.ticks_per_readout,
    }
};
local noise_source = {
    type: "NoiseSource",
    data: params.daq {
        model: wc.tn(noise_model),
	anode: wc.tn(anode_nominal),
        start_time: params.daq.start_time,
        stop_time: params.daq.stop_time,
        readout_time: params.daq.readout_time,
    }
};
local noise = if params.sim.noise then [noise_model, noise_source] else [];

local drifter = {
    type : "Drifter",
    data : params.lar + params.sim  {
        anode: wc.tn(anode_nominal),
    }
};

local ductor_nominal = {
    type : 'Ductor',
    name : 'nominal',
    data : params.daq + params.lar + params.sim {
        nsigma : 3,
	anode: wc.tn(anode_nominal),
    }
};
local ductor_uvground = ductor_nominal {
    name : 'uvground',
    data : super.data {
        anode: wc.tn(anode_uvground),
    }
};
local ductor_vyground = ductor_nominal {
    name : 'vyground',
    data : super.data {
        anode: wc.tn(anode_vyground),
    }
};

local uboone_ductor_chain = [
    {
        ductor: wc.tn(ductor_uvground),
        rule: "wirebounds",
        args: [ 
            [ { plane:0, min:296, max:296 } ],
            [ { plane:0, min:298, max:315 } ],
            [ { plane:0, min:317, max:317 } ],
            [ { plane:0, min:319, max:327 } ],
            [ { plane:0, min:336, max:337 } ],
            [ { plane:0, min:343, max:345 } ],
            [ { plane:0, min:348, max:351 } ],
            [ { plane:0, min:376, max:400 } ],
            [ { plane:0, min:410, max:445 } ],
            [ { plane:0, min:447, max:484 } ],
            [ { plane:0, min:501, max:503 } ],
            [ { plane:0, min:505, max:520 } ],
            [ { plane:0, min:522, max:524 } ],
            [ { plane:0, min:536, max:559 } ],
            [ { plane:0, min:561, max:592 } ],
            [ { plane:0, min:595, max:598 } ],
            [ { plane:0, min:600, max:632 } ],
            [ { plane:0, min:634, max:652 } ],
            [ { plane:0, min:654, max:654 } ],
            [ { plane:0, min:656, max:671 } ],
        ],
    },

    {
        ductor: wc.tn(ductor_vyground),
        rule: "wirebounds",
        args: [
            [ { plane:2, min:2336, max:2399 } ],
            [ { plane:2, min:2401, max:2414 } ],
            [ { plane:2, min:2416, max:2463 } ],
        ],
    },
    {               // catch all if the above do not match.
        ductor: wc.tn(ductor_nominal),
        rule: "bool",
        args: true,
    },

];
local multi_ductor = {
    type: "MultiDuctor",
    data : {
        anode: wc.tn(anode_nominal),
        chains : [
            uboone_ductor_chain,
        ],
    }
};
local ductor = multi_ductor;
local signal = [drifter, ductor_nominal, ductor_vyground, ductor_uvground, ductor];

local frame_summer = {
    type: "FrameSummer",
};

local digitizer = {
    type: "Digitizer",
    data : params.adc {
        anode: wc.tn(anode_nominal),
        frame_tag: "sim",
    }
};

local saver = {
    type: "wclsFrameSaver",
    data: {
        anode: wc.tn(anode_nominal),
        digitize: true,         // true means save as RawDigit, else recob::Wire
        frame_tags: ["sim"],
        nticks: params.daq.ticks_per_readout,
        chanmaskmaps: [],
    }
};

// not a configurable
local sink = { type: "DumpFrames" };

local numpy_saver = {
    data: params.daq {
        filename: "bogosim-%(noise)s.npz" % {
            noise: if params.sim.noise then "noise" else "signal",
        },
        frame_tags: [""],       // untagged.
        scale: 1.0,             // ADC
    }
};
local numpy_depo_saver = numpy_saver { type: "NumpyDepoSaver" };
local numpy_frame_saver = numpy_saver { type: "NumpyFrameSaver" };

local io = [saver, numpy_depo_saver, numpy_frame_saver];

local graph_noise = [
    {
        tail: { node: wc.tn(source) },
        head: { node: wc.tn(numpy_depo_saver) },
    },
    {
        tail: { node: wc.tn(numpy_depo_saver) },
        head: { node: wc.tn(drifter) },
    },
    {
        tail: { node: wc.tn(drifter) },
        head: { node: wc.tn(ductor) },
    },
    {
        tail: { node: wc.tn(ductor) },
        head: { node: wc.tn(frame_summer), port:0 },
    },
    {
        tail: { node: wc.tn(noise_source) },
        head: { node: wc.tn(frame_summer), port:1 },
    },
    {
        tail: { node: wc.tn(frame_summer) },
        head: { node: wc.tn(digitizer) },
    },
    {
        tail: { node: wc.tn(digitizer) },
        head: { node: wc.tn(saver) },
    },
    {
        tail: { node: wc.tn(saver) },
        head: { node: wc.tn(numpy_frame_saver) },
    },
    {
        tail: { node: wc.tn(numpy_frame_saver) },
        head: { node: wc.tn(sink) },
    },
];

local graph_quiet = [
    {
        tail: { node: wc.tn(source) },
        head: { node: wc.tn(numpy_depo_saver) },
    },
    {
        tail: { node: wc.tn(numpy_depo_saver) },
        head: { node: wc.tn(drifter) },
    },
    {
        tail: { node: wc.tn(drifter) },
        head: { node: wc.tn(ductor) },
    },
    {
        tail: { node: wc.tn(ductor) },
        head: { node: wc.tn(digitizer) },
    },
    {
        tail: { node: wc.tn(digitizer) },
        head: { node: wc.tn(saver) },
    },
    {
        tail: { node: wc.tn(saver) },
        head: { node: wc.tn(numpy_frame_saver) },
    },
    {
        tail: { node: wc.tn(numpy_frame_saver) },
        head: { node: wc.tn(sink) },
    },
];

local graph = if params.sim.noise then graph_noise else graph_quiet;

local app = {
    type: "Pgrapher",
    data: { edges: graph }
};


// the final configuration sequence of data structures

[random, source] + anodes + noise + signal + [frame_summer, digitizer] + io + [app]
