#include "Equalizer.h"

#include "FilterNode.h"

namespace ma {

Equalizer::Equalizer(const NodeGraph& nodeGraph)
    : _nodeGraph(nodeGraph),
      _input(_nodeGraph),
      _output(_nodeGraph) {
    _input.attach(_output);
}

Equalizer::~Equalizer() {
}

void Equalizer::doAttach(const NodeGraph& other) {
    ma_node_attach_output_bus(const_cast<ma_node_base*>(&_output._nodeBase), 0,
                              const_cast<ma_node_base*>(&other._endpoint), 0);
}

void Equalizer::addFilter(const FilterConfig& config) {
    _filters.push_back(std::make_unique<FilterNode>(_nodeGraph, config));

    if (_filters.size() == 1) {
        // If first filter, attach input node to this filter
        _input.attach(*_filters.back());
    } else {
        // Else attach previous filter to this filter
        _filters.at(_filters.size()-2)->attach(*_filters.back());
    }

    // Attach this filter to endpoint
    _filters.back()->attach(_output);
}

void Equalizer::setFilters(const std::vector<FilterConfig>& filterConfigs) {
    for (size_t i = 0; i < filterConfigs.size(); ++i) {
        if (_filters.size() > i) {
            *_filters.at(i) = filterConfigs.at(i);
        } else {
            addFilter(filterConfigs.at(i));
        }
    }
    for (size_t i = filterConfigs.size(); i < _filters.size(); ++i) {
        *_filters.at(i) = FilterConfig();
    }
}

/*
void Equalizer::doAttach(const Node& other) {
    ma_node_attach_output_bus(const_cast<ma_node_base*>(&_input._nodeBase), 0,
                              const_cast<ma_node_base*>(&other._nodeBase), 0);
}
*/

/*
void Equalizer::doAttach(const NodeGraph& other) {
    ma_node_attach_output_bus(const_cast<ma_node_base*>(&_nodeBase), 0,
                              const_cast<ma_node_base*>(&other._endpoint), 0);
}
*/

} // namespace ma
