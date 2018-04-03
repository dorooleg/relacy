#ifndef RL_SNAPSHOT_SLAVE_HPP
#define RL_SNAPSHOT_SLAVE_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <cstdlib>
#include "snapshot_protocol.hpp"
#include "snapshot_utility.hpp"
#include <iostream>
namespace rl
{


class snapshot_slave
{
private:
    class empty_functor
    {
    public:
        void operator()() { }
    };

public:
    static void make_snapshot(unsigned int port, uint64_t id, bool finish = false, bool stop = false)
    {
        make_snapshot(port, id, empty_functor(), finish, stop);
    }

    template<typename T>
    static void make_snapshot(unsigned int port, uint64_t id, T functor, bool finish = false, bool stop = false)
    {
        while (true)
        {
            pid_t pid = !stop ? fork() : 0;

            if (pid == 0)
            {

                int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
                struct hostent *server = gethostbyname("localhost");
                struct sockaddr_in server_addr = {};
                server_addr.sin_family = AF_INET;
                bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
                server_addr.sin_port = htons(port);

                if (connect(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
                {
                    exit(0);
                }

                strong_write(socket_fd, reinterpret_cast<char*>(&id), sizeof(id));
                strong_write(socket_fd, reinterpret_cast<char*>(&pid), sizeof(pid));
                strong_write(socket_fd, reinterpret_cast<char*>(&finish), sizeof(finish));
                strong_write(socket_fd, reinterpret_cast<char*>(&stop), sizeof(stop));

                snapshot_event event = snapshot_event_start;
                strong_read(socket_fd, reinterpret_cast<char*>(&event), sizeof(event));

                close(socket_fd);

                if (event == snapshot_event_stop)
                {
                    exit(0);
                }

                functor();

                if (stop)
                {
                    break;
                }
            }
            else
            {
                if (finish)
                {
                    exit(0);
                }

                break;
            }
        }
    }

};

}

#endif

