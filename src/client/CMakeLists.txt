add_executable(client client.cc)
target_include_directories(client PUBLIC ../../include/network)
target_link_libraries(client PUBLIC network)
target_include_directories(client PUBLIC ../network)
