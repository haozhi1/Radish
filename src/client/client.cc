#include <iostream>
#include <string>
#include <memory>
#include "socket.h"
#include "connection.h"

// Simple client for testing connection and protocol parsing.
int main(int argc, char *argv[]) {
    if(argc < 2) {
        std::cout << "Please enter an address in the format of address:port\n";
        return EXIT_FAILURE;
    }
    std::string addr_arg {argv[1]};
    auto idx = addr_arg.find_first_of(':');
    std::string addr {addr_arg.begin(), addr_arg.begin() + idx};
    std::string port {addr_arg.begin() + idx + 1, addr_arg.end()};
    auto client_socket {std::make_unique<radish::network::Socket>(AF_INET, SOCK_STREAM, 0)};
    if(client_socket->InitAndConnect(addr, std::stoi(port)) != 0) {
        std::cout << "Could not connec to host: " << addr << ", port: " << port << '\n';
        return EXIT_FAILURE;
    }
    radish::network::Connection server_conn {std::move(client_socket)};
    while(true) {
        std::cout << addr_arg << "> ";
        std::string cmd{};
        std::getline(std::cin >> std::ws, cmd);
        
        server_conn.WriteBufferAppend(cmd);
        server_conn.TransitionTo(radish::network::ConnState::kWrite);
        while(server_conn.TryWrite());
        
        server_conn.TransitionTo(radish::network::ConnState::kRead);
        while(server_conn.TryRead());
        auto buff {server_conn.GetReadBuffer()};
        server_conn.EraseReadBuffer(buff.size());
        std::string resp {buff.begin(), buff.end()};
        std::cout << resp;
        
        if (server_conn.AtState(radish::network::ConnState::kEnd)) {
            return EXIT_SUCCESS;
        }
    }
}