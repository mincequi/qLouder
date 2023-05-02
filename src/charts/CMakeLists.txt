add_library(charts STATIC
    AbstractChart.cpp
    ChartModel.cpp
    FrChartModel.cpp
    IrChartModel.cpp
    LinChart.cpp
    LogChart.cpp
    TimeTable.cpp
)

target_link_libraries(charts
PRIVATE
  common
  measure
  Qt${QT_VERSION_MAJOR}::Core
)
