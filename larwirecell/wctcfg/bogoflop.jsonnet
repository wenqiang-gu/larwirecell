local wc = import "wirecell.jsonnet";

local source = {
    type: "wclsSimDepoSource",
};


local sink = {                  // note, not configurable
    type: "DumpDepos",
};

local app = {
    type: "Pgrapher",
    data: {
        edges:     [
            {
                tail: { node: wc.tn(source) },
                head: { node: wc.tn(sink) },
            }
        ],
    }
};

[source, app]
