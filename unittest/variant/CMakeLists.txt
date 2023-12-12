rust_enum_add_unittest(variant_test
        SOURCES
            bad_variant_access_test.cpp
            variant_size_test.cpp
            variant_alternative_test.cpp
            monostate_test.cpp
            get_if_test.cpp
            get_test.cpp
            holds_alternative_test.cpp
            comparison_test.cpp
            comparison_three_way_test.cpp
            assignment_test/converting_assignment_test.cpp
            assignment_test/copy_assignment_test.cpp
            assignment_test/move_assignment_test.cpp
            constructor_test/converting_constructor_test.cpp
            constructor_test/copy_constructor_test.cpp
            constructor_test/move_constructor_test.cpp
            constructor_test/default_constructor_test.cpp
            constructor_test/in_place_index_constructor_test.cpp
            constructor_test/in_place_index_init_list_constructor_test.cpp
            constructor_test/in_place_type_constructor_test.cpp
            constructor_test/in_place_type_init_list_constructor_test.cpp
            p0608r3_test.cpp
            destructor_test.cpp
            emplace_test/index_emplace_test.cpp
            emplace_test/index_init_list_emplace_test.cpp
            emplace_test/type_emplace_test.cpp
            emplace_test/type_init_list_emplace_test.cpp
            index_test.cpp
            valueless_by_exception_test.cpp
            visit_test/visit_test.cpp
            visit_test/return_visit_test.cpp
            visit_test/p2162r2_test.cpp
            $<$<BOOL:${RUST_ENUM_ENABLE_HEAVY_TESTING}>:big_variant_test.cpp>
            swap_test.cpp
        LIBS ${CMAKE_PROJECT_NAME}::rust_enum)

target_include_directories(variant_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (MSVC)
    target_compile_options(variant_test PRIVATE /bigobj)
endif()
