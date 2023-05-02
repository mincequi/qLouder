cmake_minimum_required(VERSION 3.12)

pkg_check_modules(TAGLIB REQUIRED taglib)

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.0-preview4
  GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(Catch2)

# Fft test
#add_executable(fft_test
#  FftTest.cpp
#  ../src/Config.cpp
#  ../src/FrequencyTable.cpp
#  ../src/status/StatusModel.cpp
#)

#target_include_directories(fft_test
#PRIVATE
#  ../src
#  ${FFTW3_INCLUDE_DIRS}
#)
#
#target_link_libraries(fft_test
#PRIVATE
#  pthread
#  Qt${QT_VERSION_MAJOR}::Core
#  Catch2::Catch2WithMain
#  ${FFTW3F_LINK_LIBRARIES}
#  ${FFTW3_LINK_LIBRARIES}
#  measure
#)

# FrequencyTable test
add_executable(FrequencyTableTest
  FrequencyTableTest.cpp
  ../src/common/FrequencyTable.cpp
)
target_include_directories(FrequencyTableTest
PRIVATE
  ../src
)
target_link_libraries(FrequencyTableTest
PRIVATE
  Catch2::Catch2WithMain
)

# MeasurementTest
add_executable(MeasurementTest
  MeasurementTest.cpp
)
target_include_directories(MeasurementTest
PRIVATE
  ../src
)
target_link_libraries(MeasurementTest
PRIVATE
  Catch2::Catch2WithMain
  measure
)

# TimeTable test
add_executable(TimeTableTest
  TimeTableTest.cpp
  ../src/charts/TimeTable.cpp
)
target_include_directories(TimeTableTest
PRIVATE
  ../src
)
target_link_libraries(TimeTableTest
PRIVATE
  Catch2::Catch2WithMain
)

# Crossover test
add_executable(xo_test
  CrossoverTest.cpp
)
target_include_directories(xo_test
PRIVATE
  ../src
)
target_link_libraries(xo_test
PRIVATE
  common
  Catch2::Catch2WithMain
)

# Project test
add_executable(project_test
  ProjectTest.cpp
)
target_include_directories(project_test
PRIVATE
  ../src
  ../3rdParty
  ${TAGLIB_INCLUDE_DIRS}
)
target_link_libraries(project_test
PRIVATE
  Catch2::Catch2WithMain
  ${TAGLIB_LINK_LIBRARIES}
)
