add_library(settings STATIC
    FrFile.cpp
    SettingsModel.cpp
)

target_link_libraries(settings
PRIVATE
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Multimedia
)
