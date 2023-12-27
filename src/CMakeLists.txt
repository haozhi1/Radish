add_executable(main main.cc)

target_include_directories(main PUBLIC ../include/network)

# Server lib
add_library(socket STATIC network/socket.cc network/server.cc)
target_include_directories(socket PUBLIC ../include/network)
target_link_libraries(main PUBLIC socket)
