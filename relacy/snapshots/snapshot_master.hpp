#ifndef RL_SNAPSHOT_MASTER_HPP
#define RL_SNAPSHOT_MASTER_HPP

#include <sys/socket.h>
#include <stdexcept>
#include <netinet/in.h>
#include <unistd.h>
#include "snapshot.hpp"
#include "snapshot_utility.hpp"
#include <iostream>


namespace rl
{

class snapshot_master
{
public:
    snapshot_master(unsigned short port)
        : port_(port)
    {
        socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);

        if (socket_fd_ < 0)
        {
            throw std::invalid_argument("ERROR opening socket");
        }

        const int optval = 1;
        setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

        struct sockaddr_in server_addr = { };
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port_);

        if (bind(socket_fd_, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        {
            throw std::invalid_argument("ERROR on binding");
        }

        listen(socket_fd_, 5);
    }

public:
    snapshot wait_snapshot()
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int socket_client_fd = accept(socket_fd_, (struct sockaddr *) &client_addr, &client_len);

        if (socket_client_fd < 0)
        {
            throw std::invalid_argument("Problem with accept client");
        }

        uint64_t id = 0;
        strong_read(socket_client_fd, reinterpret_cast<char*>(&id), sizeof(id));

        pid_t pid = 0;
        strong_read(socket_client_fd, reinterpret_cast<char*>(&pid), sizeof(pid));

        bool is_end = false;
        strong_read(socket_client_fd, reinterpret_cast<char*>(&is_end), sizeof(is_end));

        bool is_stop = false;
        strong_read(socket_client_fd, reinterpret_cast<char*>(&is_stop), sizeof(is_stop));

        return snapshot(id, pid, socket_client_fd, is_end, is_stop);
    }


    int start()
    {
        pid_t pid = fork();

        if (pid == 0)
        {
            close(socket_fd_);
        }

        return pid;
    }

    ~snapshot_master()
    {
        close(socket_fd_);
    }

private:
    const unsigned short port_;
    int socket_fd_;
};

}

#endif
