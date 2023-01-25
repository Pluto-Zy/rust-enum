cmake_minimum_required(VERSION 3.23)

set(FORMAT_SOURCE_FILES)
set(FOLDERS include src unittest)

foreach (_FOLDER ${FOLDERS})
    set(ALL_FILES hpp h hh cpp c cc cxx)
    list(TRANSFORM ALL_FILES PREPEND "${FORMAT_BASE_DIR}/${_FOLDER}/*.")
    file(GLOB_RECURSE SOURCE_FILES FOLLOW_SYMLINKS
            LIST_DIRECTORIES false ${ALL_FILES})
    list(APPEND FORMAT_SOURCE_FILES ${SOURCE_FILES})
endforeach()

message(STATUS "Start to format source files.")
execute_process(COMMAND "${CLANG-FORMAT_PATH}" -i --style=file ${FORMAT_SOURCE_FILES})
