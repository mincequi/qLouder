add_library(player_miniaudio STATIC
    Equalizer.cpp
    Engine.cpp
    FilterConfig.cpp
    FilterNode.cpp
    Node.cpp
    NodeGraph.cpp
    Player.cpp
    Sound.cpp
    ${miniaudio_SOURCE_DIR}/extras/miniaudio_split/miniaudio.c
)

target_include_directories(player_miniaudio
PUBLIC
    ${miniaudio_SOURCE_DIR}/extras/miniaudio_split
)

target_compile_options(player_miniaudio
PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Werror -Wno-deprecated-declarations> # -Wall -Wextra -Wpedantic
)

target_link_libraries(player_miniaudio
PRIVATE
    dl
    pthread
)
