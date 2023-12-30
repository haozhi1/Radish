#include <iostream>
#include "server.h"
#include "logger.h"

int main() {
    radish::network::Server server;
    server.SetPort(7802);
    return server.Start();
}