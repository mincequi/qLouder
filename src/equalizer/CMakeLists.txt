add_library(equalizer STATIC
    AbstractStrategy.cpp
    EqualizerModel.cpp
    FilterModel.cpp
    LowHighPassStrategy.cpp
    NoneStrategy.cpp
    PeakingStrategy.cpp
    ShelvingStrategy.cpp
)

target_link_libraries(equalizer
PUBLIC
    Qt${QT_VERSION_MAJOR}::Qml
PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    rxcpp
)
