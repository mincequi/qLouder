#pragma once

#include <cstdint>
#include <miniaudio.h>

#include "NodeTemplate.h"

namespace ma {

class NodeGraph {
public:
    NodeGraph(const ma_node_graph& nodeGraphBase);
    ~NodeGraph();

private:
    const ma_node_graph& _nodeGraphBase;
    const ma_node_base& _endpoint;

    friend class Equalizer;
    friend class Node;
    friend class FilterNode;
};

} // namespace ma
