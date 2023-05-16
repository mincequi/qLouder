add_library(common STATIC
  AudioFilter.cpp
  Config.cpp
  Fft.cpp
  FrequencyTable.cpp
  ImpulseResponse.cpp
  MonoSignal.cpp
  UiUtil.cpp
)

target_include_directories(common
PUBLIC
  ${FFTW3_INCLUDE_DIRS}
)

target_link_libraries(common
PRIVATE
#TODO: remove Qt dependency from common
  Qt${QT_VERSION_MAJOR}::Core
  ${FFTW3F_LINK_LIBRARIES}
  ${FFTW3_LINK_LIBRARIES}
)
