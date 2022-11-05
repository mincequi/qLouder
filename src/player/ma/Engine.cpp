#include "Engine.h"

namespace ma {

Engine::Engine() :
    // An engine is actually a node graph. However, if we attch it to another
    // node we want the other node to attach to the engine's endpoint, so the
    // engine's base is its endpoint.
    NodeGraph(this->nodeGraph) {
    ma_engine_init(NULL, this);
}

Engine::~Engine() {
    ma_engine_uninit(this);
}

} // namespace ma
