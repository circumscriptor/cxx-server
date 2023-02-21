add_executable(cxx - server)

    add_subdirectory(data) add_subdirectory(commands)

        set(HEADERS_SERVER connection.hxx ctf.hxx datastream.hxx enet.hxx handler.hxx manager.hxx map.hxx peer.hxx protocol.hxx server.hxx)

            target_sources(cxx - server PRIVATE $ { HEADERS_SERVER } main.cxx)

                target_precompile_headers(cxx - server PRIVATE $ { HEADERS_SERVER })

                    target_compile_features(cxx - server PRIVATE cxx_std_23)

                        target_link_libraries(cxx - server PRIVATE CONAN_PKG::glm CONAN_PKG::enet)
