#include <iostream>
#include "server.h"

int main() {
    radish::network::Server server;
    server.SetPort(9876);
    return server.Start();
}