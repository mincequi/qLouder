FetchContent_Declare(
  r8brain
  GIT_REPOSITORY https://github.com/avaneev/r8brain-free-src.git
  GIT_TAG        version-5.7
)
FetchContent_MakeAvailable(r8brain)

add_library(measure STATIC
    AudioBuffer.cpp
    AudioDeviceInfo.cpp
    ExcitationSignal.cpp
    InverseFilter.cpp
    MeasurementError.cpp
    MeasureModel.cpp
    Measurement.cpp
    MeasurementManager.cpp
    MeasurementService.cpp
    ResponseSignal.cpp
    Signal.cpp
    SignalGenerator.cpp
    ${r8brain_SOURCE_DIR}/r8bbase.cpp
)

target_include_directories(measure
PRIVATE
    ${FFTW3_INCLUDE_DIRS}
    ${r8brain_SOURCE_DIR}
)

target_link_libraries(measure
PRIVATE
    Qt${QT_VERSION_MAJOR}::Multimedia
    ${FFTW3F_LINK_LIBRARIES}
    ${FFTW3_LINK_LIBRARIES}
    pthread
    rxcpp
)
