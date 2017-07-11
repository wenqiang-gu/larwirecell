// This is a Wire Cell Toolkit configuration file in Jsonnet format.
// It holds configuration useful for executing the WCT from larsoft.
// To use it, import it into your own Jsonnet configuration file and
// reference and/or override some of its data structures.

local wc = import "wirecell.jsonnet";
local anodes = import "multi/anodes.jsonnet";

{
    source : {
        frame: {                // choose one, they are mutually
                                // exclusive so no need to give them
                                // unique names.

            raw : {
                // This is a source of WCT IFrame objects made from RawDigits.
                // Default source is "daq".
                type: "wclsRawFrameSource",
                data: {
                    source_label: "daq", 
                }
            },

            // This is a source of WCT IFrame objects made from
            // RawDigits by the original WCT noise filter module.
            noisefiltered : {
                type: "wclsRawFrameSource",
                data: {
                    source_label: "raw::RawDigits_wcNoiseFilter__DataRecoStage1",
                }
            },
        },

    },

    sink : {
        frame: {
            // A cooked frame sink accepts a frame and saves it into
            // art::Event as a vector of recob::Wire.  It needs an
            // anode plane in order to resolve what plane a channel is
            // in.
            cooked : {
                type: "wclsCookedFrameSink",
                data: {
                    anode: wc.tn(anodes.nominal),
                }
            },
        },
    }

}
