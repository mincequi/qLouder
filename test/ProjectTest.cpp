#include <catch2/catch_test_macros.hpp>

#include <taglib/wavfile.h>
//#include <taglib/fileref.h>
#include <taglib/tpropertymap.h>

TEST_CASE("Read metadata", "[Project]") {
    TagLib::RIFF::WAV::File f("/Users/minski/Documents/untitled.wav");
    auto propertyMap = f.tag()->properties();
    //f.InfoTag()->properties()
    std::cerr << std::endl << "Property count: " << propertyMap.size() << std::endl;
    for (const auto& kv : propertyMap) {
        std::cerr << kv.first << ": " << std::endl;
    }
}
