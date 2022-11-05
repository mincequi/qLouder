#pragma once

#include <string>
#include <miniaudio.h>
#include "NodeGraph.h"

namespace ma {

class Engine;

// Sound is actually an EngineNode, but is actually the only one inheriting
// from EnginNode. So, let's drop this intermediate layer of inheriting.
class Sound : public Node {
public:
    Sound(Engine& engine,
          const std::string& filePath);
    ~Sound();

    float length() const;

    void start();
    void stop();

    bool isPlaying() const;

    void setLooping(bool isLooping);
    float loopBegin() const;
    void setLoopBegin(float seconds);
    float loopEnd() const;
    void setLoopEnd(float seconds);

    float cursor() const;
    void seekTo(float seconds);

    uint32_t sampleRate() const;

private:
    void doAttach(const Node& other) override;

    ma_sound _sound;

    friend class Player;
};

} // namespace ma
