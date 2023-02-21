
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
    grenade.hxx
    intel.hxx
    line.hxx
    name.hxx
    player.hxx
    position.hxx
    ray.hxx
    score.hxx
    spawn.hxx
    team.hxx
    weapon.hxx
)

target_sources(cxx-server
    PRIVATE
        ${HEADERS_SERVER_DATA}
)

target_precompile_headers(cxx-server
    PRIVATE
        ${HEADERS_SERVER_DATA}
)
