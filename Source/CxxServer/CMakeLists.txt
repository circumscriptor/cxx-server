
project_add_executable(CxxServer
    HEADERS
        Protocol.hpp
        Server.hpp
    SOURCES
        Main.cpp
        Protocol.cpp
        Server.cpp
    LIBRARIES
        CONAN_PKG::glm
        CONAN_PKG::enet
)

project_generate_headers(CxxServer
    FROM_PROJECT
)

#
# Add subdirectories
#

add_subdirectory(data)
add_subdirectory(commands)
add_subdirectory(Details)
