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
    ${PROJECT_SOURCE_DIR}/source
    # USE_STAMP_FILE
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Generating documentation"
    # ALL
)
