#include "Sound.h"

#include <Engine.h>

namespace ma {

Sound::Sound(Engine& engine, const std::string& filePath)
    : Node(_sound.engineNode.baseNode) {
    ma_sound_init_from_file(&engine, filePath.c_str(), MA_SOUND_FLAG_DECODE, NULL, NULL, &_sound);

    // We must explicitely set the loop points
    uint64_t length;
    ma_sound_get_length_in_pcm_frames(&_sound, &length);
    ma_data_source_set_loop_point_in_pcm_frames(_sound.pDataSource, 0, length);
}

Sound::~Sound() {
    ma_sound_uninit(&_sound);
}

void Sound::start() {
    ma_sound_start(&_sound);
}

void Sound::stop() {
    ma_sound_stop(&_sound);
}

bool Sound::isPlaying() const {
    return ma_sound_is_playing(&_sound);
}

void Sound::setLooping(bool isLooping) {
    ma_sound_set_looping(&_sound, isLooping);
}

float Sound::loopBegin() const {
    uint64_t begin = 0;
    ma_data_source_get_loop_point_in_pcm_frames(_sound.pDataSource, &begin, NULL);
    return (float)begin / sampleRate();
}

void Sound::setLoopBegin(float seconds) {
    uint64_t end;
    ma_data_source_get_loop_point_in_pcm_frames(_sound.pDataSource, NULL, &end);
    uint64_t begin = seconds * sampleRate();
    ma_data_source_set_loop_point_in_pcm_frames(_sound.pDataSource, begin, end);
    uint64_t cursor;
    ma_data_source_get_cursor_in_pcm_frames(_sound.pDataSource, &cursor);
    if (cursor < begin) {
        ma_data_source_seek_to_pcm_frame(_sound.pDataSource, begin);
    }
}

float Sound::loopEnd() const {
    uint64_t end = 0;
    ma_data_source_get_loop_point_in_pcm_frames(_sound.pDataSource, NULL, &end);
    return (float)end / sampleRate();
}

void Sound::setLoopEnd(float seconds) {
    uint64_t begin;
    ma_data_source_get_loop_point_in_pcm_frames(_sound.pDataSource, &begin, NULL);
    uint64_t end_ = seconds * sampleRate();
    ma_data_source_set_loop_point_in_pcm_frames(_sound.pDataSource, begin, end_);
    uint64_t cursor;
    ma_data_source_get_cursor_in_pcm_frames(_sound.pDataSource, &cursor);
    if (cursor > end_) {
        ma_data_source_seek_to_pcm_frame(_sound.pDataSource, begin);
    }
}

float Sound::length() const {
    float length = 0.0;
    ma_sound_get_length_in_seconds(const_cast<ma_sound*>(&_sound), &length);
    return length;
}

float Sound::cursor() const {
    float cursor = 0.0;
    ma_sound_get_cursor_in_seconds(const_cast<ma_sound*>(&_sound), &cursor);
    return cursor;
}

void Sound::seekTo(float seconds) {
    ma_sound_seek_to_pcm_frame(&_sound, seconds * sampleRate());
}

uint32_t Sound::sampleRate() const {
    uint32_t sampleRate_ = 0;
    ma_sound_get_data_format(const_cast<ma_sound*>(&_sound), NULL, NULL, &sampleRate_, NULL, 0);
    return sampleRate_;
}

void Sound::doAttach(const Node& other) {
    ma_node_attach_output_bus(&_sound, 0,
                              const_cast<ma_node_base*>(&other._nodeBase), 0);
}

} // namespace ma
