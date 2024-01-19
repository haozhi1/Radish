add_executable(main main.cc)
target_include_directories(main PUBLIC ./network)
target_include_directories(main PUBLIC ./util)

# Server lib
add_subdirectory(network)
target_link_libraries(main PUBLIC network)

# Client
add_subdirectory(client)