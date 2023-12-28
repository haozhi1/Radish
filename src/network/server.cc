#include <sys/socket.h>
#include <iostream>
#include <utility>
#include "poll.h"
#include "connection.h"
#include "socket.h"
#include "server.h"
#include "logger.h"

namespace radish::network {

int Server::Start() {
    Socket server_socket {AF_INET, SOCK_STREAM, 0};
    server_socket.CreateAndBind(port_);
    server_socket.Listen();
    if(!SetNonBlocking(server_socket)) {
        LOG(ERROR) << "could not start server";
        return -1;
    }

    LOG(INFO) << "server listening at port: " << port_;
    // Event loop.
    std::unordered_map<int, std::unique_ptr<Connection>> conn_map {};
    std::vector<pollfd> fd_pool {};
    while(true) {
        // Build pool
        fd_pool.clear();
        fd_pool.push_back({server_socket.getFd(), POLLIN, 0});

        for(auto& [fd, conn] : conn_map) {
            if(conn->getState() == ConnState::kEnd) {
                conn_map.erase(fd);
            } else {
                int event = conn->getState() == ConnState::kRequest ? POLLIN : POLLOUT;
                fd_pool.push_back({fd, POLLIN, 0});
            }
        }

        // Blocking poll
        int active_count {poll(&fd_pool[0], fd_pool.size(), 1000)};
        if(active_count < 1) continue;

        // New connection
        if(fd_pool[0].revents) {
            auto new_sock = server_socket.Accept();
            if(SetNonBlocking(*new_sock)) {
                int fd {new_sock->getFd()};
                auto ptr = std::make_unique<Connection>(std::move(new_sock));
                conn_map.insert({fd, std::move(ptr)});
            }
        }

        // Handle IO
        for(auto i {1}; i < fd_pool.size(); ++i) {
            auto io_pollfd = fd_pool[i];
            if(io_pollfd.revents) {
                HandleConnection(*conn_map[io_pollfd.fd]);
            }
        }
    }
}

void Server::SetPort(int port) {
    port_ = port;
}

} // namespace radish::network