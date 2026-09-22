add_executable(nova_ui_architecture_test
    ui_architecture_runtime.c
)

target_link_libraries(nova_ui_architecture_test PRIVATE nova_ui_runtime)

enable_testing()
add_test(NAME nova_ui_architecture_runtime COMMAND nova_ui_architecture_test)

