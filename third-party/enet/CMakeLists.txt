add_library(enet STATIC)
add_library(third-party::enet ALIAS enet)

target_compile_definitions(enet
    PRIVATE
        $<$<PLATFORM_ID:Windows>:_WINSOCK_DEPRECATED_NO_WARNINGS>
)

target_compile_features(enet
    PUBLIC
        c_std_99
)

target_include_directories(enet
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(enet
    SYSTEM
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(WIN32)
    target_link_libraries(enet
        PRIVATE
            winmm
            ws2_32
    )
endif()

target_sources(enet
    PRIVATE
        source/callbacks.c
        source/compress.c
        source/host.c
        source/list.c
        source/packet.c
        source/peer.c
        source/protocol.c
        $<IF:$<PLATFORM_ID:Windows>,source/win32.c,source/unix.c>
)
