add_library(player STATIC
    EqualizerNode.cpp
    PlayerBarModel.cpp
    PlayerModel.cpp
    PlayerModelPrivate.cpp
)

target_include_directories(player
PRIVATE
    ${TAGLIB_INCLUDE_DIRS}
)

target_link_libraries(player
PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Multimedia
    ${TAGLIB_LINK_LIBRARIES}
    cinder
    rxcpp
)
