find_program(CLANG-FORMAT_PATH clang-format REQUIRED)

# Print the version of clang-format
execute_process(
        COMMAND "${CLANG-FORMAT_PATH}" --version
        OUTPUT_VARIABLE CLANG_FORMAT_VERSION_OUTPUT
)

message(STATUS "Found Clang-Format - ${CLANG_FORMAT_VERSION_OUTPUT}")

string(REGEX MATCH "clang-format version ([0-9]+\\.[0-9]+\\.[0-9]+)" _
        "${CLANG_FORMAT_VERSION_OUTPUT}")
set(CLANG_FORMAT_VERSION "${CMAKE_MATCH_1}")
