#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fstream>
#include <iostream>

#include <measure/AudioBuffer.h>
#include <measure/ExcitationSignal.h>
#include <measure/Farina.h>
#include <measure/InverseSignal.h>
#include <measure/Measurement.h>
#include <measure/MeasurementFactory.h>
#include <measure/SignalGenerator.h>

#include <wavpack/wavpack.h>

std::vector<uint8_t> out;

static int block_out(std::vector<uint8_t>* output, uint8_t* data, int32_t byte_count) {
    if (output != &out) return 1;

    size_t oldSize = output->size();
    output->resize(oldSize + byte_count);
    std::memcpy(output->data() + oldSize, data, byte_count);
    return 1;
}

Measurement loadWv() {
    auto ctx = WavpackOpenFileInput("ir_1.wv", nullptr, 0, 0);
    REQUIRE(ctx != nullptr);

    REQUIRE(WavpackGetSampleRate(ctx) == 44100);
    REQUIRE(WavpackGetNumChannels(ctx) == 1);
    REQUIRE(WavpackGetBytesPerSample(ctx) == 4);
    REQUIRE(WavpackGetBitsPerSample(ctx) == 32);
    //REQUIRE(WavpackGetNumSamples(ctx) == 22050);
    REQUIRE(WavpackGetMode(ctx) & MODE_FLOAT);
    REQUIRE(WavpackGetMode(ctx) & MODE_LOSSLESS);
    //REQUIRE(WavpackGetMode(ctx) & MODE_WVC);

    std::vector<float> ir;
    ir.resize(WavpackGetNumSamples(ctx));
    WavpackUnpackSamples(ctx, (int32_t*)ir.data(), ir.size());

    return Measurement(44100, ir);
}

TEST_CASE("Read metadata", "[Measurement]") {
    AudioBuffer buffer(QIODevice::ReadOnly);
    auto farina = Farina(&buffer);
    auto signal = farina.excitationSignal();

    std::vector<float> recorded;
    recorded.resize(signal.data().size()/2);
    for (size_t i = 0; i < signal.data().size(); i += 2) {
        auto s = sin(((i/2.0)/recorded.size()) * M_PI);
        recorded.at(i/2) = signal.data().at(i) * s;
    }

    auto measurement = Measurement(44100, farina.impulseResponse(recorded));
    auto ir = measurement.windowedIr();

    MeasurementFactory::toDisk(measurement, "ir_1.wv");

    auto measurement2 = loadWv();
    auto fr1 = measurement.calibratedFr();
    auto fr2 = measurement2.calibratedFr();

    for (size_t i = 0; i < fr1.size(); ++i) {
        REQUIRE_THAT(fr1.at(i), Catch::Matchers::WithinAbs(fr2.at(i), 0.1));
    }
}
