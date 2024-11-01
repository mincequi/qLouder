#include "ProjectFactory.h"

#include <cstring>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <wavpack/wavpack.h>

#include "Project.h"

Project* ProjectFactory::fromDisk(const std::string& fileName) {
    auto ctx = WavpackOpenFileInput(fileName.c_str(), nullptr, 0, 0);

    ImpulseResponse ir;
    ir.resize(WavpackGetNumSamples(ctx));
    WavpackUnpackSamples(ctx, (int32_t*)ir.data(), ir.size());

    return new Project(WavpackGetSampleRate(ctx), ir);
}

void ProjectFactory::toDisk(Project& project, const std::string& fileName) {

    std::vector<uint8_t> out;
    auto wpc = WavpackOpenFileOutput([](void* id, void* data, int32_t byte_count) {
        auto output = static_cast<std::vector<uint8_t>*>(id);
        size_t oldSize = output->size();
        output->resize(oldSize + byte_count);
        std::memcpy(output->data() + oldSize, data, byte_count);
        return 1;
    }, &out, nullptr);

    WavpackConfig config = {0};
    config.bytes_per_sample = 4;
    config.bits_per_sample = 32;
    config.channel_mask = 4; // Microsoft standard: 4 == mono, 3 == stereo
    config.num_channels = 1;
    config.sample_rate = project.sampleRate();
    config.float_norm_exp = 127;
    config.qmode |= QMODE_RAW_PCM;
    //config.flags = CONFIG_VERY_HIGH_FLAG | CONFIG_HYBRID_FLAG | CONFIG_HYBRID_SHAPE;
    WavpackSetConfiguration(wpc, &config, project.ir().size());
    WavpackPackInit(wpc);
    WavpackPackSamples(wpc, (int32_t*)project.ir().data(), project.ir().size());
    WavpackFlushSamples(wpc);

    std::string str;
    float f = 1.0f;
    for (int i = 0; i < 256; ++i) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << f;
        str += ss.str();
        if (i != 255) str += '\0';
        f *= 1.02f;
    }
    WavpackAppendTagItem(wpc, "qLouder MicrophoneCalibration0", str.data(), str.size());
    WavpackWriteTag(wpc);

    std::fstream file;
    file = std::fstream(fileName.c_str(), std::ios::out | std::ios::binary);
    file.write((char*)out.data(), out.size());
    file.close();
}
