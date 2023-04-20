project_new_target(
    TARGET cxxserver
    NAMESPACE cxxserver
    EXECUTABLE
    SOURCES_COMMON
        Server.cpp
    SOURCES_TARGET
        main.cxx
    LINK_PRIVATE
        CONAN_PKG::glm
        CONAN_PKG::enet
        CONAN_PKG::bitsery
    NO_TESTS
)

# project_add_executable(cxxserver
#     HEADERS
#         Connection.hpp
#         Protocol.hpp
#         Server.hpp
#     SOURCES
#         Connection.cpp
#         Main.cpp
#         Protocol.cpp
#         Server.cpp
#     LIBRARIES
#         CONAN_PKG::glm
#         CONAN_PKG::enet
#         CONAN_PKG::bitsery
# )

#
# Add subdirectories
#

# add_subdirectory(data)
# add_subdirectory(commands)
# add_subdirectory(Details)
