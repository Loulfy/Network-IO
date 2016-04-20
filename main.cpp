#include <iostream>

#include "TcpServer.hpp"

using namespace std;

int main()
{
    cout << "-- START --" << endl;

    nio::TcpServer server(6666);

    nio::TcpServer::Event e;

    do
    {
        e = server.poll();

        switch(e)
        {
            case nio::TcpServer::Event::CONNECT:
                cout << "client" << endl;
                break;
        }
    }
    while(e != 42);

    return EXIT_SUCCESS;
}