#ifndef PLAYERMODELPRIVATE_H
#define PLAYERMODELPRIVATE_H

#include <memory>
#include <string>

#include <player/EqualizerNode.h>

namespace cinder {
namespace audio {
class BufferPlayerNode;
class EqualizerNode;
}
}

class PlayerModelPrivate {
public:
    PlayerModelPrivate();

    bool setFile(const std::string& file);
    void setDevice(const std::string& output);

    std::vector<std::string> supportedFileExtensions() const;
    bool isPlaying() const;
    void play();
    void stop();

    double loopBeginTime() const;
    void setLoopBeginTime(double value);

    double loopEndTime() const;
    void setLoopEndTime(double value);

    double progressTime() const;
    double totalTime() const;

    void setFilters(const std::vector<cinder::audio::EqualizerNode::Filter>& filters);

    void toggleEqualizer();
    bool isEqualizerEnabled();

private:
    std::shared_ptr<cinder::audio::BufferPlayerNode> _bufferPlayer;
    std::shared_ptr<cinder::audio::EqualizerNode> _equalizer;
};

#endif // PLAYERMODELPRIVATE_H
