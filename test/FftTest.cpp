#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <measure/Measurement.h>
#include <measure/SignalGenerator.h>

#include <algorithm>

TEST_CASE("fade in, fade out", "[SignalGenerator]") {
    std::vector<float> signal(100, 1.0f);
    SignalGenerator::fadeIn(signal.begin(), SignalGenerator::Left, SignalGenerator::Hann, 20);
    SignalGenerator::fadeOut(signal.end(), SignalGenerator::Left, SignalGenerator::Hann, 20);

    REQUIRE(signal.at(0) == 0.0f);
    REQUIRE(signal.at(1) == 1.0f);
    REQUIRE(signal.at(38) < 1.0f);
    REQUIRE(signal.at(39) == 1.0f);
    REQUIRE(signal.at(40) == 1.0f);

    REQUIRE(signal.at(80) == 1.0f);
    REQUIRE(signal.at(82) < 1.0f);
    REQUIRE(signal.at(99) < 0.0f);
}

TEST_CASE("FFT with FFTW3", "[FFTW3]") {
    // 250 ms
    {
        std::vector<float> signal(123000);
        std::vector<float> filter(123000);
        Measurement<float> measurement(48000, 20.0, 20000.0, signal, filter);
        BENCHMARK("FFT 250ms with FFTW<float>") {
            //auto out = measurement.impulseResponse();
        };
    }

    {
        std::vector<double> signal(123000);
        std::vector<double> filter(123000);
        Measurement<double> measurement(48000, 20.0, 20000.0, signal, filter);
        BENCHMARK("FFT 250ms with FFTW<double>") {
            //auto out = measurement.impulseResponse();
        };
    }

    {
        std::vector<float> signal;
        SignalGenerator::sineSweep(signal, 250, 48000, 20.0, 20000.0);
        std::vector<float> filter = signal;
        std::reverse(filter.begin(), filter.end());
        SignalGenerator::volumeEnvelope(filter.begin(), filter.end(), 20.0, 20000.0);
        Measurement<float> measurement(48000, 20.0, 20000.0, signal, filter);
        BENCHMARK("FFT 250ms with FFTW<float>> odd") {
            //auto out = measurement.impulseResponse();
        };
    }
}

double fRand(double fMin, double fMax) {
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

TEST_CASE("Full measurement cycle", "[FFTW3]") {
    std::vector<float> signal;
    SignalGenerator::sineSweep(signal, 250, 48000, 20.0, 20000.0);
    std::vector<float> filter = signal;
    std::reverse(filter.begin(), filter.end());
    SignalGenerator::volumeEnvelope(filter.begin(), filter.end(), 20.0, 20000.0);
    Measurement<float> measurement(48000, 20.0, 20000.0, signal, filter);

    BENCHMARK("Impulse response window") {
        measurement.setIrWindowLeft(fRand(-200.0, -1.0));
        measurement.setIrWindowRight(fRand(1, 800.0));
        return measurement.irWindow();
    };
    BENCHMARK("Windowed impulse response") {
        measurement.setIrWindowLeft(fRand(-200.0, -1.0));
        measurement.setIrWindowRight(fRand(1, 800.0));
        return measurement.windowedIr();
    };
    BENCHMARK("Frequency response") {
        measurement.setIrWindowLeft(fRand(-200.0, -1.0));
        measurement.setIrWindowRight(fRand(1, 800.0));
        return measurement.windowedIr();
    };
}
