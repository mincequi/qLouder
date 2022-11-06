add_library(player STATIC
    PlayerBarModel.cpp
    PlayerModel.cpp
)

target_include_directories(player
PRIVATE
    ${TAGLIB_INCLUDE_DIRS}
)

add_subdirectory(ma)

target_link_libraries(player
PRIVATE
    equalizer
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Multimedia
    ${TAGLIB_LINK_LIBRARIES}
    player_miniaudio
    rxcpp
)
