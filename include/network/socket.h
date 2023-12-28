#ifndef NETWORK_SOCKET_H
#define NETWORK_SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <unistd.h>
#include <vector>


namespace radish::network {

// RAII wrapper for socket so I don't have to deal with C array and close(fd).
class Socket {
public:
    Socket(int domain, int type, int protocol): 
        domain_{domain}, 
        type_{type}, 
        protocol_{protocol} {}
    ~Socket() { Close(); };
    
    // No copy
    Socket(const Socket& socket) = delete;
    Socket& operator=(const Socket& socket) = delete;

    int CreateAndBind(int port);
    int Listen();
    std::unique_ptr<Socket> Accept();
    int Read(std::vector<char>& buff, int size, int offset);
    int Write(const std::vector<char>& buff, int size);
    int Close() { return close(fd_); };

    bool isBound() const { return fd_ != -1;};
    int getFd() const { return fd_; };
private:
    int fd_ {-1};
    int domain_ {AF_INET};
    int type_ {SOCK_STREAM};
    int protocol_{0};
    int backlog_{20};
};

int SetNonBlocking(const Socket & socket);

} // namespace radish

#endif