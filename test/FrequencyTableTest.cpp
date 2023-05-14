#include <cmath>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

#include <common/FrequencyTable.h>

TEST_CASE("default table has 241 values", "[FrequencyTable]") {
    FrequencyTable<double> table;
    REQUIRE(table.frequencies().size() == 241);
}

TEST_CASE("1/3 octave table has 31 values", "[FrequencyTable]") {
    FrequencyTable<double> table(3);
    REQUIRE(table.frequencies().size() == 31);
}

TEST_CASE("table starting at 16 Hz has 249 values", "[FrequencyTable]") {
    FrequencyTable<double> table(24, 16.0);
    REQUIRE(table.frequencies().size() == 249);
}

TEST_CASE("table starting at 16 Hz and ending at 25000 has 257 values", "[FrequencyTable]") {
    FrequencyTable<double> table(24, 16.0, 25000.0);
    REQUIRE(table.frequencies().size() == 257);
}

TEST_CASE("table extends values at start and end", "[FrequencyTable]") {
    FrequencyTable<double> table;
    std::map<double, double> map { { 30.7, 1.0}, { 31.5, 2.0}, { 32.5, 5.0} };
    auto result = table.interpolate(map, false);
    REQUIRE(result.at(0) == 1.0);
    REQUIRE(result.at(240) == 5.0);
}

TEST_CASE("match exact values", "[FrequencyTable]") {
    FrequencyTable<double> table;

    std::map<double, double> map { { 30.7, 1.0}, { 31.5, 2.0}, { 32.5, 5.0} };
    auto result = table.interpolate(map, false);
    REQUIRE_THAT(result, Catch::Matchers::Contains(std::vector<double>{1.0, 2.0, 5.0}));
}

TEST_CASE("interpolate single higher value", "[FrequencyTable]") {
    FrequencyTable<double> table;

    std::map<double, double> map { { 30.7, 1.0}, { 31.51, 2.0}, { 32.5, 5.0} };
    auto result = table.interpolate(map, false);

    REQUIRE(result.at(15) == 1.0);
    REQUIRE((result.at(16) > 1.9 && result.at(16) < 2.0));
    REQUIRE(result.at(17) == 5.0);
}

TEST_CASE("interpolate single lower value", "[FrequencyTable]") {
    FrequencyTable<double> table;

    std::map<double, double> map { { 30.7, 1.0}, { 31.49, 2.0}, { 32.5, 5.0} };
    auto result = table.interpolate(map, false);

    REQUIRE(result.at(15) == 1.0);
    REQUIRE((result.at(16) > 2.0 && result.at(16) < 2.1));
    REQUIRE(result.at(17) == 5.0);
}

TEST_CASE("interpolate single missing value", "[FrequencyTable]") {
    FrequencyTable<double> table;

    std::map<double, double> map { { 30.7, 1.0}, { 32.5, 5.0} };
    auto result = table.interpolate(map, false);

    REQUIRE(result.at(15) == 1.0);
    REQUIRE((result.at(16) > 2.8 && result.at(16) < 3.2));
    REQUIRE(result.at(17) == 5.0);
}
