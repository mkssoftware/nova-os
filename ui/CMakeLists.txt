add_library(nova_ui_runtime STATIC
    src/runtime.c
)

target_include_directories(nova_ui_runtime PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(nova_ui_runtime PUBLIC c_std_17)

