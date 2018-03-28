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
public:
    static void make_snapshot(unsigned int port, uint64_t id, bool finish = false)
    {
        pid_t pid = fork();

        if (pid != 0)
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

            snapshot_event event = snapshot_event_start;
            strong_read(socket_fd, reinterpret_cast<char*>(&event), sizeof(event));

            if (event == snapshot_event_stop)
            {
                exit(0);
            }
        }
    }

};

}

#endif

