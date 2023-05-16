add_library(project STATIC
  Project.cpp
  ProjectFactory.cpp
  ProjectManager.cpp
  ProjectModel.cpp
  ProjectsModel.cpp
)

target_link_libraries(project
PRIVATE
  common
  measure
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Multimedia
  Qt${QT_VERSION_MAJOR}::Qml
)
