#include "NodeGraph.h"

namespace ma {

NodeGraph::NodeGraph(const ma_node_graph& nodeGraphBase) :
    _nodeGraphBase(nodeGraphBase),
    _endpoint(nodeGraphBase.endpoint) {
}

NodeGraph::~NodeGraph() {
}

} // namespace ma
