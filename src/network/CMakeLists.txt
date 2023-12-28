add_library(network STATIC 
    socket.cc 
    server.cc 
    connection.cc)
target_include_directories(network PUBLIC ../../include/network)
target_include_directories(network PUBLIC ../../include/util)
