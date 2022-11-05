#include "Node.h"

#include "Engine.h"
#include "Equalizer.h"
#include "NodeGraph.h"

namespace ma {

Node::Node(const ma_node_base& nodeBase) :
    _nodeBase(nodeBase) {
}

Node::~Node() {
}

void Node::doAttach(const Node& other) {
    ma_node_attach_output_bus(const_cast<ma_node_base*>(&_nodeBase), 0,
                              const_cast<ma_node_base*>(&other._nodeBase), 0);
}

void Node::doAttach(const NodeGraph& other) {
    ma_node_attach_output_bus(const_cast<ma_node_base*>(&_nodeBase), 0,
                              const_cast<ma_node_base*>(&other._endpoint), 0);
}

void Node::doAttach(const Equalizer& other) {
    ma_node_attach_output_bus(const_cast<ma_node_base*>(&_nodeBase), 0,
                              const_cast<ma_node_base*>(&other._input._nodeBase), 0);
}

} // namespace ma
