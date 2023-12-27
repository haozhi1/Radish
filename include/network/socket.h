#ifndef NETWORK_SOCKET_H
#define NETWORK_SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <unistd.h>
#include <string>


namespace radish {
    
class Socket {
public:
    Socket(int domain, int type, int protocol);
    ~Socket() { close(fd_); };
    
    // No copy
    Socket(const Socket& socket) = delete;
    Socket& operator=(const Socket& socket) = delete;

    int Bind(int port);
    int Listen();
    std::unique_ptr<Socket> Accept();
    std::string Read();
    int Write(const std::string& msg);

    bool isBound() { return fd_ != -1;}
private:
    int fd_ {-1};
    int domain_ {AF_INET};
    int type_ {SOCK_STREAM};
    int protocol_{0};
    int backlog_{20};
};

} // namespace radish

#endif