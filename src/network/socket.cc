#include <iostream>
#include "socket.h"

constexpr int kMaxBufferSize = 4096;

namespace radish {
    
Socket::Socket(int domain, int type, int protocol): 
    domain_{domain}, 
    type_{type}, 
    protocol_{protocol} {}

int Socket::Bind(int port) {
    int status {0};
    if(isBound()) return status;
    if((status = fd_ = socket(domain_, type_, protocol_)) < 0) {
        std::cout << "create socket error: " << gai_strerror(errno) << '\n';
        return status;
    }
    int opt = 1;
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

std::string Socket::Read() {
    std::string res;
    std::vector<char> tmp_buf(kMaxBufferSize);
    ssize_t recv_count {0};
    while((recv_count = recv(fd_, &tmp_buf[0], tmp_buf.size(), 0)) != 0) {   
        if(recv_count == -1) {
            std::cout << "read error: " << gai_strerror(errno) << '\n';
            return {};
        }
        res.append(tmp_buf.begin(), tmp_buf.end());
    }
    return res;
}

int Socket::Write(const std::string& msg) {
    const char* buf = msg.c_str();
    int status = send(fd_, buf, msg.size(), 0);
    if(status < 0) {
        std::cout << "write error: " << gai_strerror(errno) << '\n';
    }
    return status;
}

} // namespace radish