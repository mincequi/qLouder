#include "PlayerModelPrivate.h"

#include <cinder/app/AppBase.h>
#include <cinder/audio/Context.h>
#include <cinder/audio/Device.h>
#include <cinder/audio/FilterNode.h>
#include <cinder/audio/OutputNode.h>
#include <cinder/audio/Source.h>
#include <cinder/audio/Voice.h>

#include "EqualizerNode.h"

#define ctx cinder::audio::master()

using namespace cinder;
using namespace cinder::app;
using namespace cinder::audio;

PlayerModelPrivate::PlayerModelPrivate() {
    const auto devices = cinder::audio::Device::getOutputDevices();
    std::vector<std::string> deviceNames;
    for (const auto& d : devices) {
        const std::string name = d->getName();
        deviceNames.push_back(name);
        std::cerr << "name: " << name << std::endl;
    }
    assert(deviceNames.size());

    _bufferPlayer = ctx->makeNode<BufferPlayerNode>();
    _bufferPlayer->setLoopEnabled(true);
    //ctx->setOutput(dynamic_pointer_cast<audio::OutputDeviceNode>(Device::findDeviceByName(deviceNames.front())));

    _equalizer = ctx->makeNode<EqualizerNode>();

    _bufferPlayer >> _equalizer >> ctx->getOutput();
    ctx->enable();
}

bool PlayerModelPrivate::setFile(const std::string& file) {
    SourceFileRef sourceFile = load(loadFile(file));
    _bufferPlayer->loadBuffer(sourceFile);
    return true;
}

void PlayerModelPrivate::setDevice(const std::string& output) {
    const auto device = dynamic_pointer_cast<audio::OutputDeviceNode>(Device::findDeviceByName(output));
    device->setAutoEnabled();
    ctx->setOutput(device);

    _bufferPlayer >> ctx->getOutput();
}

std::vector<std::string> PlayerModelPrivate::supportedFileExtensions() const {
    return SourceFile::getSupportedExtensions();
}

bool PlayerModelPrivate::isPlaying() const {
    return _bufferPlayer->isEnabled();
}

void PlayerModelPrivate::play() {
    const auto pos = _bufferPlayer->getReadPosition();
    _bufferPlayer->start();
    _bufferPlayer->seek(pos);
}

void PlayerModelPrivate::stop() {
    const auto pos = _bufferPlayer->getReadPosition();
    _bufferPlayer->stop();
    _bufferPlayer->seek(pos);
}

double PlayerModelPrivate::begin() const {
    return _bufferPlayer->getLoopBeginTime() / _bufferPlayer->getNumSeconds();
}

void PlayerModelPrivate::setBegin(double value) {
    if (value > progress()) {
        _bufferPlayer->seekToTime(value * _bufferPlayer->getNumSeconds());
    }
    _bufferPlayer->setLoopBeginTime(_bufferPlayer->getNumSeconds()*value);
}

double PlayerModelPrivate::end() const {
    return _bufferPlayer->getLoopEndTime() / _bufferPlayer->getNumSeconds();
}

void PlayerModelPrivate::setEnd(double value) {
    _bufferPlayer->setLoopEndTime(_bufferPlayer->getNumSeconds()*value);
}

double PlayerModelPrivate::progress() const {
    return _bufferPlayer->getReadPositionTime() / _bufferPlayer->getNumSeconds();
}

void PlayerModelPrivate::setFilters(const std::vector<cinder::audio::EqualizerNode::Filter>& filters) {
    _equalizer->setFilters(filters);
}
