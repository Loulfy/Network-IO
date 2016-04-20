#include "TcpServer.hpp"

#include <iostream>

using namespace std;

namespace nio
{
    TcpServer::TcpServer(int port) : m_socket(socket(AF_INET, SOCK_STREAM, 0))
    {
        // Create TCP socket
        if(m_socket < 0)
        {
            throw std::runtime_error("NIO::TCPSERVER::SOCKET::FAILED");
        }

        int on = 1;

        // Allow socket to be reuseable
        if(setsockopt(m_socket, SOL_SOCKET,  SO_REUSEADDR, &on, sizeof(int)) < 0)
        {
            throw std::runtime_error("NIO::TCPSERVER::SETSOCKOPT::FAILED");
        }

        // Set socket to be non-blocking
        if(ioctl(m_socket, FIONBIO, &on) < 0)
        {
            throw std::runtime_error("NIO::TCPSERVER::IOCTL::FAILED");
        }

        // Bind socket
        m_ip = {0};
        m_ip.sin_family      = AF_INET;
        m_ip.sin_addr.s_addr = htonl(INADDR_ANY);
        m_ip.sin_port        = htons(port);

        if(bind(m_socket, (sockaddr*)(&m_ip), sizeof(m_ip)) < 0)
        {
            throw std::runtime_error("NIO::TCPSERVER::BIND::FAILED");
        }

        // Listen
        if(listen(m_socket, 32) < 0)
        {
            throw std::runtime_error("NIO::TCPSERVER::LISTEN::FAILED");
        }

        // Init poll
        struct pollfd server;
        server.fd       = m_socket;
        server.events   = POLLIN;

        m_clients.push_back(server);
    }

    TcpServer::~TcpServer()
    {
        close(m_socket);
        for(pollfd s : m_clients)
        {
            if(s.fd >= 0)
            {
                close(s.fd);
            }
        }
    }

    TcpServer::Event TcpServer::poll()
    {
        int rc = ::poll(m_clients.data(), m_clients.size(), 180000);

        if(rc < 0)
        {
            throw std::runtime_error("NIO::TCPSERVER::POLL::FAILED");
        }

        if(rc == 0)
        {
            return TIMEOUT;
        }

        for(pollfd s : m_clients)
        {
            if(s.revents == 0)
                continue;

            if(s.revents != POLLIN)
            {
                throw std::runtime_error("NIO::TCPSERVER::POLL::REVENTS::ERROR");
            }

            // Add Clients
            if(s.fd == m_socket)
            {
                int s_client = accept(m_socket, NULL, NULL);
                if(s_client < 0)
                {
                    if(errno != EWOULDBLOCK)
                    {
                        throw std::runtime_error("NIO::TCPSERVER::ACCEPT::FAILED");
                    }
                    break;
                }

                struct pollfd client;
                client.fd       = s_client;
                client.events   = POLLIN;

                m_clients.push_back(client);
                return CONNECT;

            }
            else
            {
                //send(s.fd, "LOL\n");

            }
        }

        return TIMEOUT;
    }

    void TcpServer::send(int client, string data)
    {
        ::send(client, data.data(), data.size(), 0);
    }
}
