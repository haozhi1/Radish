#include <iostream>
#include "socket.h"
#include <fcntl.h>

namespace radish::network {

int SetNonBlocking(const Socket& socket) {
    if(!socket.isBound()) return -1;
    int fd {socket.getFd()};
    int flags {fcntl(fd, F_GETFL, 0)};
    if (flags == -1) return -1;
    int status {fcntl(fd, F_SETFL, flags | O_NONBLOCK)};
    return status;
}

int Socket::CreateAndBind(int port) {
    int status {0};
    if(isBound()) return status;
    if((status = fd_ = socket(domain_, type_, protocol_)) < 0) {
        std::cout << "create socket error: " << gai_strerror(errno) << '\n';
        return status;
    }
    int opt {1};
    if((status = setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) < 0) {
        std::cout << "set socket option error: " << gai_strerror(errno) << '\n';
        return status;
    }

    sockaddr_in addr_in {};
    addr_in.sin_family = domain_;
    addr_in.sin_addr.s_addr = INADDR_ANY;
    addr_in.sin_port = htons(port);
    
    if((status = bind(fd_, reinterpret_cast<sockaddr*>(&addr_in), sizeof(addr_in))) < 0) {
        std::cout << "bind error: " << gai_strerror(errno) << '\n';
    }
    return status;
}

int Socket::Listen() {
    int status = listen(fd_, backlog_);
    if(status < 0) {
        std::cout << "listen error: " << gai_strerror(errno) << '\n';
    }
    return status;
}

std::unique_ptr<Socket> Socket::Accept() {
    sockaddr client_addr {};
    socklen_t size = sizeof(client_addr);
    int new_fd {-1};
    if((new_fd = accept(fd_, &client_addr, &size)) < 0) {
        return nullptr;
    }
    auto new_socket = std::make_unique<Socket>(domain_, type_, protocol_);
    new_socket->fd_ = new_fd;
    return new_socket;
}

int Socket::Read(std::vector<char>& buff, int size, int offset) {
    int recv_count = {static_cast<int>(recv(fd_, &buff[offset], size, 0))};
    if(recv_count < 0) {
        std::cout << "write error: " << gai_strerror(errno) << '\n';
    }
    return recv_count;
}

int Socket::Write(const std::vector<char>& buff, int size) {
    int sent_count {static_cast<int>(send(fd_, &buff[0], size, 0))};
    if(sent_count < 0) {
        std::cout << "write error: " << gai_strerror(errno) << '\n';
    }
    return sent_count;
}

} // namespace radish::network