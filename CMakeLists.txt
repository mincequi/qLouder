cmake_minimum_required(VERSION 3.14)

Include(FetchContent)

FetchContent_Declare(
  rxcpp
  GIT_REPOSITORY https://github.com/ReactiveX/RxCpp.git
  GIT_TAG        761b932a80e2be6e2b62d232e754bd96fc448946 # main from Mar 13, 2022
)
FetchContent_MakeAvailable(rxcpp)

#FetchContent_Declare(
#  rpp
#  GIT_REPOSITORY https://github.com/victimsnino/ReactivePlusPlus.git
#  GIT_TAG        v0.0.5
#)
#FetchContent_MakeAvailable(rpp)

FetchContent_Declare(
  r8brain
  GIT_REPOSITORY https://github.com/avaneev/r8brain-free-src.git
  GIT_TAG        version-5.7
)
FetchContent_MakeAvailable(r8brain)

set (CINDER_DISABLE_ANTTWEAKBAR ON CACHE INTERNAL "CINDER_DISABLE_ANTTWEAKBAR")
set (CINDER_DISABLE_IMGUI ON CACHE INTERNAL "CINDER_DISABLE_IMGUI")
set (CINDER_DISABLE_VIDEO ON CACHE INTERNAL "CINDER_DISABLE_VIDEO")
FetchContent_Declare(
  cinder
  GIT_REPOSITORY https://github.com/cinder/Cinder.git
  GIT_TAG        bfd055817bc6f04b4ff6a410932618c7eef0d78c
)
FetchContent_MakeAvailable(cinder)

#FetchContent_Declare(
#  taglib
#  GIT_REPOSITORY https://github.com/taglib/taglib.git
#  GIT_TAG        v1.12
#)
#FetchContent_MakeAvailable(taglib)

project(qLouder VERSION 0.3 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick QuickControls2 Multimedia Charts Svg Xml REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick QuickControls2 Multimedia Charts Svg Xml REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFTW3F REQUIRED fftw3f)
pkg_check_modules(FFTW3 REQUIRED fftw3)
string(REPLACE "fftw3f" "fftw3f_threads" FFTW3F_THREADS_LINK_LIBRARIES ${FFTW3F_LINK_LIBRARIES})
string(REPLACE "fftw3" "fftw3_threads" FFTW3_THREADS_LINK_LIBRARIES ${FFTW3_LINK_LIBRARIES})

include_directories(
    ${r8brain_SOURCE_DIR}
    src
)

set(PROJECT_SOURCES
    src/charts/AbstractChart.cpp
    src/charts/ChartModel.cpp
    src/charts/FrChartModel.cpp
    src/charts/IrChartModel.cpp
    src/charts/LinChart.cpp
    src/charts/LogChart.cpp
    src/charts/TimeTable.cpp
    src/crossover/CrossoverModel.cpp
    src/equalizer/EqualizerLogic.cpp
    src/equalizer/EqualizerModel.cpp
    src/equalizer/FilterModel.cpp
    src/measure/AudioBuffer.cpp
    src/measure/AudioDeviceInfo.cpp
    src/measure/ExcitationSignal.cpp
    src/measure/InverseFilter.cpp
    src/measure/MeasurementError.cpp
    src/measure/MeasureModel.cpp
    src/measure/Measurement.cpp
    src/measure/MeasurementManager.cpp
    src/measure/MeasurementService.cpp
    src/measure/ResponseSignal.cpp
    src/measure/Signal.cpp
    src/measure/SignalGenerator.cpp
    src/player/EqualizerNode.cpp
    src/player/PlayerBarModel.cpp
    src/player/PlayerModel.cpp
    src/player/PlayerModelPrivate.cpp
    src/project/ProjectModel.cpp
    src/status/StatusModel.cpp
    src/target/TargetModel.cpp
    src/ui/UiUtil.cpp
    src/util/FrFile.cpp
    src/AudioFilter.cpp
    src/Config.cpp
    src/FrequencyTable.cpp
    src/MaterialDesignIcon.cpp
    src/main.cpp
    src/main.cpp
    src/qml.qrc
    ${r8brain_SOURCE_DIR}/r8bbase.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(qLouder
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET qLouder APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(qLouder SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(qLouder
          ${PROJECT_SOURCES}
        )
    endif()
endif()

if(APPLE)
    set_target_properties(qLouder PROPERTIES
	MACOSX_BUNDLE TRUE
    )
endif()

target_include_directories(qLouder
PRIVATE
    ${FFTW3_INCLUDE_DIRS}
)

target_compile_definitions(qLouder
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(qLouder
PRIVATE
    pthread
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::QuickControls2
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::Charts
    Qt${QT_VERSION_MAJOR}::Svg
    Qt${QT_VERSION_MAJOR}::Xml
    ${FFTW3F_LINK_LIBRARIES}
    ${FFTW3_LINK_LIBRARIES}
    #${FFTW3F_THREADS_LINK_LIBRARIES}
    #${FFTW3_THREADS_LINK_LIBRARIES}
    rxcpp
    #rpp
    cinder
)

set_target_properties(qLouder PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(qLouder)
    qt_finalize_executable(qLouder)
endif()

add_subdirectory(test)
