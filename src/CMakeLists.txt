add_library(rust_enum INTERFACE)
target_include_directories(rust_enum INTERFACE ${CMAKE_SOURCE_DIR}/include)
add_library(${CMAKE_PROJECT_NAME}::rust_enum ALIAS rust_enum)
