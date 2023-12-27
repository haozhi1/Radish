#include <sys/socket.h>
#include <iostream>
#include "socket.h"
#include "server.h"

namespace radish {

void Server::Start() {
    radish::Socket server_socket {AF_INET, SOCK_STREAM, 0};
    server_socket.Bind(port_);
    server_socket.Listen();
    while(true) {
        auto conn = server_socket.Accept();
        std::string req = conn->Read();
        conn->Write("ACK");
        std::cout << req;
    }
}

void Server::SetPort(int port) {
    port_ = port;
}

} // namespace radish