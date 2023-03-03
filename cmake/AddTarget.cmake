if (RUST_ENUM_RUN_CLANG_FORMAT)
    include(ClangFormat)
endif()

function(rust_enum_add_executable name)
    cmake_parse_arguments(EXE "" "FOLDER" "SOURCES;LIBS" ${ARGN})

    add_executable(${name} ${EXE_SOURCES})
    target_link_libraries(${name} PRIVATE ${EXE_LIBS})

    if (MSVC)
        # For macro __cplusplus
        target_compile_options(${name} PRIVATE /Zc:__cplusplus)
        # Fix warning D9025 of cl.exe.
        string(REGEX REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else ()
        target_compile_options(${name} PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-function)
    endif ()

    if (EXE_FOLDER)
        set_target_properties(${name} PROPERTIES FOLDER ${EXE_FOLDER})
    endif ()

    if (RUST_ENUM_RUN_CLANG_FORMAT)
        add_custom_command(TARGET ${name} PRE_BUILD
                COMMAND "${CMAKE_COMMAND}"
                -DFORMAT_BASE_DIR:PATH="${CMAKE_SOURCE_DIR}"
                -DCLANG-FORMAT_PATH:PATH="${CLANG-FORMAT_PATH}"
                -P ${CMAKE_SOURCE_DIR}/cmake/RunClangFormat.cmake)
    endif()
endfunction(rust_enum_add_executable)
