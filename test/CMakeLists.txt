include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.0
)
FetchContent_MakeAvailable(googletest)

add_executable(server_test server_test.cc)

target_link_libraries(server_test PRIVATE gtest_main)

add_test(server_gtest server_test)