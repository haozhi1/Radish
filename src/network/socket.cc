#include <iostream>
#include "socket.h"
#include <fcntl.h>
#include "logger.h"

namespace radish::network {

bool SetNonBlocking(const Socket& socket) {
    if(!socket.isBound()) return false;
    int fd {socket.getFd()};
    int flags {fcntl(fd, F_GETFL, 0)};
    if (flags == -1) return false;
    if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) != 0) return false;
    return true;
}

int Socket::Init() {
    int status {0};
    if(isBound()) return status;
    if((status = fd_ = socket(domain_, type_, protocol_)) < 0) {
        LOG(ERROR) << "create socket error: " << gai_strerror(errno);
        return status;
    }
    int opt {1};
    if((status = setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) != 0) {
        LOG(ERROR) << "set socket option error: " << gai_strerror(errno);
    }
    return status;
}

int Socket::InitAndBind(int port) {
    int status {Init()};
    if(status != 0) return status;

    struct addrinfo* server_addr;
    struct addrinfo hints;
    hints.ai_family = domain_;
    hints.ai_socktype = type_;
    hints.ai_protocol = protocol_;
    hints.ai_flags = AI_PASSIVE;
    if ((status = getaddrinfo("localhost", std::to_string(port).c_str(), &hints, &server_addr)) != 0){
        LOG(ERROR) << "connect error: " << gai_strerror(errno);
        return status;
    }
    
    if((status = bind(fd_, server_addr->ai_addr, server_addr->ai_addrlen)) != 0) {
        LOG(ERROR) << "bind error: " << gai_strerror(errno);
    }
    freeaddrinfo(server_addr);
    return status;
}

int Socket::Listen() {
    int status = listen(fd_, backlog_);
    if(status != 0) {
        LOG(ERROR) << "listen error: " << gai_strerror(errno);
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

int Socket::InitAndConnect(std::string& ip, int port) {
    int status {Init()};
    if(status != 0) return status;

    struct addrinfo* server_addr;
    struct addrinfo hints;
    hints.ai_family = domain_;
    hints.ai_socktype = type_;
    hints.ai_protocol = protocol_;
    if ((status = getaddrinfo(ip.c_str(), std::to_string(port).c_str(), &hints, &server_addr)) != 0){
        LOG(ERROR) << "connect error: " << gai_strerror(errno);
        return status;
    }
    status = connect(fd_, server_addr->ai_addr, server_addr->ai_addrlen);
    freeaddrinfo(server_addr);
    if (status != 0) {
        LOG(ERROR) << "connect error: " << gai_strerror(errno);
        return status;
    }
    return status;
}

int Socket::Read(std::vector<char>& buff, int size, int offset) {
    int recv_count = {static_cast<int>(recv(fd_, &buff[offset], size, 0))};
    if(recv_count < 0) {
        LOG(ERROR) << "read error: " << gai_strerror(errno);
    }
    return recv_count;
}

int Socket::Write(const std::vector<char>& buff, int size) {
    int sent_count {static_cast<int>(send(fd_, &buff[0], size, 0))};
    if(sent_count < 0) {
        LOG(ERROR) << "write error: " << gai_strerror(errno);
    }
    return sent_count;
}

int Socket::Close() {
    int status = close(fd_);
    if(status != 0) {
        LOG(ERROR) << "close error: " << gai_strerror(errno);
    }
    return status;
}

} // namespace radish::network