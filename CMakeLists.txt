cmake_minimum_required(VERSION 3.17)

#
#
# SpadesServer
#
#

project(SpadesServer)


#
# Add main target
#

add_executable(server "")

target_link_libraries(server
    PRIVATE
        enet
        z
)

add_subdirectory(Spades)
