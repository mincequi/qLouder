cmake_minimum_required(VERSION 3.12)

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.0-preview4
)

FetchContent_MakeAvailable(Catch2)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FFTW3F REQUIRED fftw3f)

# Fft test
add_executable(fft_test
  FftTest.cpp
  ../src/FrequencyTable.cpp
  ../src/measure/Measurement.cpp
  ../src/measure/SignalGenerator.cpp
  ../src/status/StatusModel.cpp
)

target_include_directories(fft_test
PRIVATE
  ../src
  ${FFTW3_INCLUDE_DIRS}
)

target_link_libraries(fft_test
PRIVATE
  Qt${QT_VERSION_MAJOR}::Core
  Catch2::Catch2WithMain
  ${FFTW3F_LINK_LIBRARIES}
  ${FFTW3_LINK_LIBRARIES}
)

# FrequencyTable test
add_executable(ft_test
  FrequencyTableTest.cpp
  ../src/FrequencyTable.cpp
)

target_include_directories(ft_test
PRIVATE
  ../src
)

target_link_libraries(ft_test
PRIVATE
  Catch2::Catch2WithMain
)

# TimeTable test
add_executable(tt_test
  TimeTableTest.cpp
  ../src/charts/TimeTable.cpp
)

target_include_directories(tt_test
PRIVATE
  ../src
)

target_link_libraries(tt_test
PRIVATE
  Catch2::Catch2WithMain
)

# Crossover test
add_executable(xo_test
  CrossoverTest.cpp
  ../src/AudioFilter.cpp
  ../src/FrequencyTable.cpp
)

target_include_directories(xo_test
PRIVATE
  ../src
)

target_link_libraries(xo_test
PRIVATE
  Catch2::Catch2WithMain
)
