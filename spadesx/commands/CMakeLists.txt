
#
#
# SpadesX
#
#

set(HEADERS_SERVER_COMMANDS
    kill.hxx
    say.hxx
)

target_sources(server
    PRIVATE
        ${HEADERS_SERVER_COMMANDS}
)

target_precompile_headers(server
    PRIVATE
        ${HEADERS_SERVER_COMMANDS}
)
