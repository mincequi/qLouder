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
)

target_include_directories(measure
PRIVATE
    ${FFTW3_INCLUDE_DIRS}
    ${cinder_SOURCE_DIR}
)

target_link_libraries(measure
PRIVATE
    Qt${QT_VERSION_MAJOR}::Multimedia
    ${FFTW3F_LINK_LIBRARIES}
    ${FFTW3_LINK_LIBRARIES}
    rxcpp
    cinder
)
