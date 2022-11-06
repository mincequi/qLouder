add_library(export STATIC
    ExportModel.cpp
    PulseEffectsExporter.cpp
)

pkg_check_modules(GSETTINGS-QT REQUIRED gsettings-qt)

target_link_libraries(export
PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    gsettings-qt
    equalizer
)

target_include_directories(export
PRIVATE
    gsettings-qt
)
