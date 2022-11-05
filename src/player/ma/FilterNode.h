#pragma once

#include <miniaudio.h>

#include "FilterConfig.h"
#include "NodeTemplate.h"

namespace ma {

class NodeGraph;

class FilterNode : public NodeTemplate<ma_biquad_node> {
public:
    FilterNode(const NodeGraph& nodeGraph, const FilterConfig& config = FilterConfig());
    ~FilterNode();

    //FilterNode(const FilterNode&) = delete;

    FilterNode& operator=(const FilterConfig& config);
    const FilterConfig& operator=(const FilterNode& node);

private:
    void initBiquad();

    FilterConfig _config;

    friend class Player;
};

} // namespace ma
