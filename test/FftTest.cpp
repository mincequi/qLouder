#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <measure/Measurement.h>
#include <measure/SignalGenerator.h>

#include <algorithm>

TEST_CASE("fade in, fade out", "[SignalGenerator]") {
    std::vector<float> signal(100, 1.0f);
    SignalFactory::fadeIn(signal.begin(), Signal::Left, SignalFactory::Hann, 20);
    SignalFactory::fadeOut(signal.end(), Signal::Left, SignalFactory::Hann, 20);

    REQUIRE(signal.at(0) == 0.0f);
    REQUIRE(signal.at(1) == 1.0f);
    REQUIRE(signal.at(38) < 1.0f);
    REQUIRE(signal.at(39) == 1.0f);
    REQUIRE(signal.at(40) == 1.0f);

    REQUIRE(signal.at(80) == 1.0f);
    REQUIRE(signal.at(82) < 1.0f);
    REQUIRE(signal.at(99) < 0.0f);
}

double fRand(double fMin, double fMax) {
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}
