add_library(common STATIC
  AudioFilter.cpp
  Config.cpp
  FrequencyTable.cpp
  Project.cpp
  UiUtil.cpp
)

target_link_libraries(common
PRIVATE
  Qt${QT_VERSION_MAJOR}::Core
)
