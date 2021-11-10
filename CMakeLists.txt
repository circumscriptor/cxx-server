cmake_minimum_required(VERSION 3.17)

#
#
# SpadesX
#
#

project(SpadesX)

#
# Find dependencies
#

find_package(Boost REQUIRED COMPONENTS iostreams)
find_package(PkgConfig)

if(PKG_CONFIG_FOUND)
    pkg_check_modules(ENet REQUIRED IMPORTED_TARGET libenet)
    set(ENET_LIBRARY PkgConfig::ENet)
else()
    set(ENET_LIBRARY enet)
endif()

#
# Add main target
#

add_executable(server "")

target_compile_options(server
    PRIVATE
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>
)

target_link_libraries(server
    PRIVATE
        Boost::iostreams
        ${ENET_LIBRARY}
)

target_include_directories(server
    PRIVATE
        ${PROJECT_SOURCE_DIR}/spadesx
)

target_compile_features(server
    PUBLIC
        cxx_std_17
)

add_subdirectory(spadesx)

#
# Documentation generator
#

find_package(Doxygen OPTIONAL_COMPONENTS dot)

if(DOXYGEN_FOUND)

    #
    # Doxygen options
    #

    set(DOXYGEN_QUIET YES)
    set(DOXYGEN_PREDEFINED "DOXYGEN_SHOULD_SKIP_THIS")
    set(DOXYGEN_DOT_GRAPH_MAX_NODES 100)
    
    #
    # Add documentation target
    #

    doxygen_add_docs(docs
        "spadesx"
        ALL
        # USE_STAMP_FILE
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Generating documentation"
    )

else()
    message(STATUS "Doxygen not found")
endif()
