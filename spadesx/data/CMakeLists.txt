
#
#
# SpadesX
#
#

set(HEADERS_SERVER_DATA
    color.hxx
    enums.hxx
    name.hxx
    player.hxx
    position.hxx
    score.hxx
    weapon.hxx
)

target_sources(server
    PRIVATE
        ${HEADERS_SERVER_DATA}
)

target_precompile_headers(server
    PRIVATE
        ${HEADERS_SERVER_DATA}
)
