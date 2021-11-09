
#
#
# SpadesX
#
#

set(HEADERS_SERVER_DATA
    base.hxx
    color.hxx
    entity.hxx
    enums.hxx
    intel.hxx
    line.hxx
    name.hxx
    player.hxx
    position.hxx
    score.hxx
    spawn.hxx
    team.hxx
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
