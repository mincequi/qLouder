add_library(crossover STATIC
    CrossoverModel.cpp
)

target_link_libraries(crossover
PUBLIC
    Qt${QT_VERSION_MAJOR}::Charts
)
