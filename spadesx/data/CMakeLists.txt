
#
#
# SpadesX
#
#

set(HEADERS_SERVER_DATA
    color.hxx
    enums.hxx
    input.hxx
    lastkill.hxx
    name.hxx
    player.hxx
    state.hxx
    position.hxx
    score.hxx
    transform.hxx
    types.hxx
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
