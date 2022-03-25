#include <cmath>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

#include <charts/TimeTable.h>

using namespace Catch::literals;

TEST_CASE("upper and lower", "[TimeTable]") {
    REQUIRE(TimeTable::upper(550) == 600);
    REQUIRE(TimeTable::upper(549) == 550);
    REQUIRE(TimeTable::upper(500) == 550);
    REQUIRE(TimeTable::upper(499) == 500);
    REQUIRE(TimeTable::upper(480) == 500);
    REQUIRE(TimeTable::upper(479) == 480);

    REQUIRE(TimeTable::lower(199) == 190);
    REQUIRE(TimeTable::lower(200) == 190);
    REQUIRE(TimeTable::lower(201) == 200);
    REQUIRE(TimeTable::upper(199) == 200);
    REQUIRE(TimeTable::upper(200) == 220);
    REQUIRE(TimeTable::upper(201) == 220);

    REQUIRE(TimeTable::lower(10) == 9.5);
    REQUIRE(TimeTable::upper(5.5) == 6.0);
    REQUIRE(TimeTable::lower(5.5) == 5.0);
    REQUIRE(TimeTable::lower(5.0) == 4.8_a);

    REQUIRE(TimeTable::upper(1.0) == 1.1_a);
    REQUIRE(TimeTable::upper(0.9) == 1.0_a);
    REQUIRE(TimeTable::lower(1.0) == 0.9_a);
    REQUIRE(TimeTable::lower(0.9) == 0.8_a);

    REQUIRE(TimeTable::lower(0.2) == 0.1_a);
    REQUIRE(TimeTable::lower(0.1) == 0.0);
    REQUIRE(TimeTable::lower(0.0) == 0.0);
    REQUIRE(TimeTable::upper(0.0) == 0.1);
    REQUIRE(TimeTable::upper(0.1) == 0.2);
}
