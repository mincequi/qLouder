#pragma once

struct ma_node_base;

namespace ma {

class Engine;
class Equalizer;
class NodeGraph;

class Node {
public:
    Node(const ma_node_base& nodeBase);
    ~Node();

    template <class T>
    void attach(const T& other) {
        doAttach(other);
    }

private:
    virtual void doAttach(const Node& other);
    virtual void doAttach(const NodeGraph& other);
    virtual void doAttach(const Equalizer& other);

    const ma_node_base& _nodeBase;

    friend class Equalizer;
    friend class Sound;
};

} // namespace ma
