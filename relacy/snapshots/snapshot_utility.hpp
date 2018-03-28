#ifndef RL_SNAPSHOT_UTILITY_HPP
#define RL_SNAPSHOT_UTILITY_HPP

#include <cstdlib>

namespace rl
{

static inline void strong_write(int socket_fd, char *data, size_t nbytes)
{
    size_t size = nbytes;
    while (nbytes)
    {
        int n = write(socket_fd, data + size - nbytes, nbytes);

        if (n < 0)
        {
            exit(0);
        }

        nbytes -= n;
    }
}

static inline void strong_read(int socket, char *data, size_t nbytes)
{
    const size_t size = nbytes;
    while (nbytes)
    {
        int n = read(socket, data + size - nbytes, nbytes);

        if (n < 0)
        {
            exit(0);
        }

        nbytes -= n;
    }
}

}

#endif

