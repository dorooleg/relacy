#ifndef RL_SNAPSHOT_UTILITY_HPP
#define RL_SNAPSHOT_UTILITY_HPP

#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


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

static inline int get_count_open_descriptors()
{
    struct stat stats;
    int max_fd_number = getdtablesize();
    int fd_counter = 0;
    for (int i = 0; i <= max_fd_number; i++)
    {
        fstat(i, &stats);
        if (errno != EBADF)
        {
            fd_counter++;
        }
    }
    return fd_counter;
}


}

#endif

