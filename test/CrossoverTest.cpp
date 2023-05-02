#include <cmath>
#include <complex>
#include <cstdint>
#include <catch2/catch_test_macros.hpp>

#include <common/FrequencyTable.h>
#include <common/AudioFilter.h>

TEST_CASE("Mix low and high pass", "[Biquad]") {
    FrequencyTable<double> table(1);
    auto fs = table.frequencies();

    AudioFilter lp(FilterType::LowPass, 630.0, 0.0, 0.707);
    AudioFilter hp(FilterType::HighPass, 630.0, 0.0, 0.707);

    auto lpr = lp.response(fs, 1);
    auto hpr = hp.response(fs, 1);

    std::vector<double> magSum;

    for (int i = 0; i < lpr.size(); ++i) {
        magSum.push_back(20*log10(abs(lpr.at(i) + hpr.at(i))));
    }

    REQUIRE(magSum.size() == fs.size());
}
