
#
#
# SpadesX
#
#

set(HEADERS_SERVER_COMMANDS
    kill.hxx
    say.hxx
)

target_sources(cxx-server
    PRIVATE
        ${HEADERS_SERVER_COMMANDS}
)

target_precompile_headers(cxx-server
    PRIVATE
        ${HEADERS_SERVER_COMMANDS}
)
