add_executable(main main.cc)

target_include_directories(main PUBLIC ../include/network)

# Server lib
add_library(network STATIC network/socket.cc network/server.cc network/connection.cc)
target_include_directories(network PUBLIC ../include/network)
target_link_libraries(main PUBLIC network)
