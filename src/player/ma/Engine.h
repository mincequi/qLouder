#pragma once

#include <miniaudio.h>
#include "NodeGraph.h"
#include "NodeTemplate.h"

namespace ma {

class Engine : public ma_engine, public NodeGraph {
public:
    Engine();
    ~Engine();
};

} // namespace ma
