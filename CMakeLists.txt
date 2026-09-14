cmake_minimum_required(VERSION 3.16)
project(femrkit LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_subdirectory(dependencies/luau)
add_library(dobby STATIC IMPORTED)
set_target_properties(dobby PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/dependencies/dobby/libdobby.a)
target_include_directories(dobby INTERFACE ${CMAKE_SOURCE_DIR}/dependencies/dobby)
add_library(femrkit SHARED
    src/boot.cpp
    src/mem.cpp
    src/func.cpp
    src/environment/box.cpp
    src/environment/reg.cpp
    src/environment/libraries/print.cpp
    src/exploit/closure.cpp
    src/luau.cpp
)
target_include_directories(femrkit PRIVATE
    ${CMAKE_SOURCE_DIR}
    src/
    header_libraries/
    dependencies/luau/Ast/include
    dependencies/luau/Compiler/include
    dependencies/luau/VM/include
    dependencies/luau/VM/src
    dependencies/luau/Common/include
    dependencies/dobby
)
target_link_libraries(femrkit PRIVATE
    Luau.Ast
    Luau.Compiler
    Luau.VM
    dobby
    log
    android
)
target_compile_definitions(femrkit PRIVATE
    MOBILE=1
)
