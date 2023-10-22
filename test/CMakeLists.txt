enable_testing()

add_executable(test_builder test_builder.cc)
target_link_libraries(
    test_builder
    mmfilebuilder
    GTest::gtest_main)




include(GoogleTest)
gtest_discover_tests(test_builder)