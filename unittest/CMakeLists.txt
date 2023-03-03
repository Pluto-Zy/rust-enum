include(CheckGoogleTest)

set(UNITTEST_ALL_TARGET ${CMAKE_PROJECT_NAME}_all_unit_tests)
add_custom_target(${UNITTEST_ALL_TARGET})
set_target_properties(${UNITTEST_ALL_TARGET} PROPERTIES
        FOLDER "${CMAKE_PROJECT_NAME} Unit Tests")

function (rust_enum_add_unittest name)
    cmake_parse_arguments(ARG "" "" "SOURCES;LIBS" ${ARGN})
    get_target_property(folder ${UNITTEST_ALL_TARGET} FOLDER)
    rust_enum_add_executable(${name}
            FOLDER ${folder}
            SOURCES ${ARG_SOURCES}
            LIBS ${ARG_LIBS} gtest_main)
    add_dependencies(${UNITTEST_ALL_TARGET} ${name})
    gtest_discover_tests(${name})
endfunction(rust_enum_add_unittest)

add_subdirectory(variant)