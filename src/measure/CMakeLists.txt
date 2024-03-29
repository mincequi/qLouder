pkg_check_modules(WAVPACK REQUIRED wavpack)

FetchContent_Declare(
  r8brain
  GIT_REPOSITORY https://github.com/avaneev/r8brain-free-src.git
  GIT_TAG        version-5.7
)
FetchContent_MakeAvailable(r8brain)

FetchContent_Declare(
  magic_enum
  GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
  GIT_TAG        v0.8.2
)
FetchContent_MakeAvailable(magic_enum)

add_library(measure STATIC
    AudioBuffer.cpp
    AudioDeviceInfo.cpp
    ExcitationSignal.cpp
    Farina.cpp
    InverseSignal.cpp
    MeasurementError.cpp
    MeasureModel.cpp
    MeasurementService.cpp
    ResponseSignal.cpp
    Signal.cpp
    SignalGenerator.cpp
    ${r8brain_SOURCE_DIR}/r8bbase.cpp
)

target_include_directories(measure
PUBLIC
    ${WAVPACK_INCLUDE_DIRS}

PRIVATE
    ${r8brain_SOURCE_DIR}
)

target_link_libraries(measure
PUBLIC
    Qt${QT_VERSION_MAJOR}::Multimedia
    ${WAVPACK_LINK_LIBRARIES}
    rxcpp

PRIVATE
    pthread
    common
    project
    status
)
