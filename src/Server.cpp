#include "ServerManager.hpp"

int main(int argc, char** argv)
{
    unsigned short port = 2000;

    ServerManager server(port);
    
    while (true)
    {
        server.handleReceive();
    }

    return 0;
}