add_executable(xjvm_unit_tests
    string_utils_test.cpp
    byte_queue_test.cpp
    method_descriptor_test.cpp
    platform_compat_test.cpp
    safepoint_coordinator_test.cpp
    gc_heap_test.cpp
    string_intern_gc_test.cpp
    native_registry_test.cpp
    exception_dispatch_test.cpp
    atomic_ops_test.cpp
    java_thread_state_test.cpp
    monitor_instruction_test.cpp
    object_monitor_wait_notify_test.cpp
)

target_link_libraries(xjvm_unit_tests PRIVATE
    xjvm_core
    gtest_main
)

target_compile_features(xjvm_unit_tests PRIVATE cxx_std_14)

add_test(NAME xjvm_unit_tests COMMAND xjvm_unit_tests)
set_tests_properties(xjvm_unit_tests PROPERTIES TIMEOUT 120)
