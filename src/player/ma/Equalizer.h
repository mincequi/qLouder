#pragma once

#include <list>
#include <vector>

#include "FilterNode.h"
#include "NodeGraph.h"

namespace ma {

class FilterConfig;
class FilterNode;

// Equalizer cannot inherit from Node, since it is actually not a node, but
// a collection of nodes. Another base class like SubGraph might be more
// suitable. For now, Equalizer gets its own attach() methods.
class Equalizer {
public:
    Equalizer(const NodeGraph& nodeGraph);
    ~Equalizer();

    template <class T>
    void attach(const T& other) {
        doAttach(other);
    }

    void addFilter(const FilterConfig& filterConfig);
    void removeFilter(int i);

    const FilterConfig& filter(int i) const;
    void setFilter(int i, const FilterConfig& filterConfig);

    const std::vector<FilterConfig>& filters() const;
    void setFilters(const std::vector<FilterConfig>& filterConfigs);

private:
    //virtual void doAttach(const Node& other);
    virtual void doAttach(const NodeGraph& other);

    const NodeGraph& _nodeGraph;

    FilterNode _input;
    std::vector<std::unique_ptr<FilterNode>> _filters;
    FilterNode _output;
    //std::list<FilterNode> _filters;

    friend class Node;
    friend class Player;
};

} // namespace ma
