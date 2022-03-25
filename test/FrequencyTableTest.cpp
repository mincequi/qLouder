#include <cmath>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

#include <FrequencyTable.h>

TEST_CASE("match exact values", "[FrequencyTable]") {
    FrequencyTable<double> table;

    std::map<double, double> map { { 30.7, 1.0}, { 31.5, 2.0}, { 32.5, 5.0} };
    auto result = table.interpolate(map);
    REQUIRE_THAT(result, Catch::Matchers::Contains(std::vector<double>{1.0, 2.0, 5.0}));
}

TEST_CASE("interpolate single higher value", "[FrequencyTable]") {
    FrequencyTable<double> table;

    std::map<double, double> map { { 30.7, 1.0}, { 31.51, 2.0}, { 32.5, 5.0} };
    auto result = table.interpolate(map);

    REQUIRE(result.at(15) == 1.0);
    REQUIRE((result.at(16) > 1.9 && result.at(24) < 2.0));
    REQUIRE(result.at(17) == 5.0);
}

TEST_CASE("interpolate single lower value", "[FrequencyTable]") {
    FrequencyTable<double> table;

    std::map<double, double> map { { 30.7, 1.0}, { 31.49, 2.0}, { 32.5, 5.0} };
    auto result = table.interpolate(map);

    REQUIRE(result.at(15) == 1.0);
    REQUIRE((result.at(16) > 2.0 && result.at(24) < 2.1));
    REQUIRE(result.at(17) == 5.0);
}

TEST_CASE("interpolate single missing value", "[FrequencyTable]") {
    FrequencyTable<double> table;

    std::map<double, double> map { { 30.7, 1.0}, { 32.5, 5.0} };
    auto result = table.interpolate(map);

    REQUIRE(result.at(15) == 1.0);
    REQUIRE((result.at(16) > 2.8 && result.at(24) < 3.2));
    REQUIRE(result.at(17) == 5.0);
}
