cmake_minimum_required(VERSION 3.17)

#
#
# SpadesX
#
#

project(SpadesX)

find_package(Boost REQUIRED COMPONENTS iostreams)

#
# Add main target
#

add_executable(server "")

target_link_libraries(server
    PRIVATE
        Boost::iostreams
        enet
)

add_subdirectory(Spades)
