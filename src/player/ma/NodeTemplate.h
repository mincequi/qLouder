#pragma once

#include "Node.h"

namespace ma {

template <class T>
class NodeTemplate : public T, public Node {
public:
    NodeTemplate(const ma_node_base& nodeBase)
        : Node(nodeBase) {
    }
};

} // namespace ma
