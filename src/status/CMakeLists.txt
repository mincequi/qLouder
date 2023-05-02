add_library(status STATIC
  StatusModel.cpp
)

target_link_libraries(status
PRIVATE
  crossover
  equalizer
  Qt${QT_VERSION_MAJOR}::Core
)
