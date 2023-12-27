#include <iostream>
#include "server.h"

int main() {
    radish::Server server;
    server.SetPort(9876);
    server.Start();
}