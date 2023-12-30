#
# Copyright 2023 The CXXSERVER Unknown Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

include_guard(GLOBAL)

#
# Includes
#

include(Compiler)
include(Options)

#
# Find programs
#

find_program(CLANG_FORMAT         clang-format)
find_program(CLANG_TIDY           clang-tidy)
find_program(CPPCHECK             cppcheck)
find_program(INCLUDE_WHAT_YOU_USE include-what-you-use)
find_program(IWYU_TOOL_PY         iwyu_tool.py)
find_program(LLVM_COV             llvm-cov)
find_program(LLVM_PROFDATA        llvm-profdata)

#
# Add custom targets
#

if(IWYU_TOOL_PY)
    add_custom_target(
        check-includes
        COMMAND
            ${CMAKE_COMMAND}
            -D "IWYU_TOOL_PY=${IWYU_TOOL_PY}"
            -D "COMPILE_COMMANDS_DIR=${CMAKE_BINARY_DIR}"
            -P "${CXXSERVER_SCRIPTS_DIRECTORY}/IncludeWhatYouUse.cmake"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Checking includes"
        VERBATIM
    )
endif()

if(CLANG_FORMAT)
    add_custom_target(
        format
        COMMAND
            ${CMAKE_COMMAND}
            -D "CLANG_FORMAT=${CLANG_FORMAT}"
            -P "${CXXSERVER_SCRIPTS_DIRECTORY}/ClangFormat.cmake"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Formatting code"
        VERBATIM
    )
endif()

# TODO: Add more

#
# Set command line variables
#

# TODO: Fix include-what-you-use options
set(INCLUDE_WHAT_YOU_USE_CLI
    ${INCLUDE_WHAT_YOU_USE}
    -Xiwyu --verbose=1
    -I ${CXXSERVER_COMPILER_INCLUDE_DIR}
)

set(CLANG_TIDY_CLI
    ${CLANG_TIDY}
    --format-style=file
)

set(CPPCHECK_CLI
    ${CPPCHECK}
    --enable=all
    --inconclusive
    --force
    --inline-suppr
    --suppress=unusedFunction
    --suppress=missingIncludeSystem
    --suppress=unmatchedSuppression
)

#
# Set property variables
#

set(CXXSERVER_PROPERTY_CLANG_TIDY           ${CLANG_TIDY_CLI}           CACHE INTERNAL "Clang-Tidy options"          )
set(CXXSERVER_PROPERTY_CPPCHECK             ${CPPCHECK_CLI}             CACHE INTERNAL "Cppcheck options"            )
set(CXXSERVER_PROPERTY_INCLUDE_WHAT_YOU_USE ${INCLUDE_WHAT_YOU_USE_CLI} CACHE INTERNAL "Include What You Use options")
