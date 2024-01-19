#ifndef NETWORK_SERVER_H
#define NETWORK_SERVER_H

namespace radish::network {

// Single threaded event loop server.
class Server {
public:
    Server() = default;
    ~Server() = default;
    int Start();
    void SetPort(int port);

    // No copy or move
    Server(const Server& server) = delete;
    Server& operator=(const Server& server) = delete;
    Server(Server&& server) = delete;
    Server& operator=(Server&& server) = delete;
private:
    int port_ {9876};
    int ttl_ {0};
    int max_conn_ {5};
};

} // namespace radish

#endif // NETWORK_SERVER_H