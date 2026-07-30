cmake_minimum_required(VERSION 3.22)

project(NovaOS
    VERSION 0.1.0
    DESCRIPTION "Nova OS - modernes modulares Betriebssystem"
    LANGUAGES C CXX ASM
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(NOVA_BUILD_BOOTLOADER "Bootloader bauen" ON)
option(NOVA_BUILD_KERNEL "Kernel bauen" ON)
option(NOVA_BUILD_TESTS "Tests bauen" OFF)

add_compile_options(
    -Wall
    -Wextra
    -Werror
)

if(NOVA_BUILD_BOOTLOADER)
    add_subdirectory(boot)
endif()

if(NOVA_BUILD_KERNEL)
    add_subdirectory(kernel)
endif()

if(NOVA_BUILD_TESTS)
    add_subdirectory(tests)
endif()