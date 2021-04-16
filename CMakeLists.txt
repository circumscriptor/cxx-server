cmake_minimum_required(VERSION 3.17)

#
#
# SpadesX
#
#

project(SpadesX)

find_package(Boost REQUIRED COMPONENTS iostreams)
find_package(PkgConfig)

#
# Add main target
#

add_executable(server "")

if(PKG_CONFIG_FOUND)
    pkg_check_modules(ENet REQUIRED IMPORTED_TARGET libenet)
    set(ENET_LIBRARY PkgConfig::ENet)
else()
    set(ENET_LIBRARY enet)
endif()

target_link_libraries(server
    PRIVATE
        Boost::iostreams
        ${ENET_LIBRARY}
)

target_include_directories(server
    PRIVATE
        ${PROJECT_SOURCE_DIR}/Spades
)

target_compile_features(server
    PUBLIC
        cxx_std_17
)

add_subdirectory(spadesx)
