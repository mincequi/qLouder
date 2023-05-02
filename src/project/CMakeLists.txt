add_library(project STATIC
  ProjectModel.cpp
  ProjectsModel.cpp
  SettingsModel.cpp
)

target_link_libraries(project
PRIVATE
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Multimedia
  Qt${QT_VERSION_MAJOR}::Qml
)
