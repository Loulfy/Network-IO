#ifndef NIO_TCPSERVER_HPP
#define NIO_TCPSERVER_HPP

#include <string>
#include <vector>
#include <cstdio>
#include <unistd.h>
#include <stdexcept>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace nio
{
    class TcpServer
    {
        public:

            enum Event {CONNECT = 1, RECEIVE = 2, DISCONNECT = 3, TIMEOUT = 0};

            TcpServer(int port);
            ~TcpServer();
            void send(int client, std::string data);

            Event poll();

        private:

            int m_socket;
            struct sockaddr_in m_ip;

            std::vector<pollfd> m_clients;
    };
}

#endif
