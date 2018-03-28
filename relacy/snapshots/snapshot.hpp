#ifndef RL_SNAPSHOT
#define RL_SNAPSHOT

#include "snapshot_utility.hpp"
#include "snapshot_protocol.hpp"
#include <sys/wait.h>

namespace rl
{


class snapshot
{
public:

    snapshot()
        : id_(0)
        , pid_(0)
        , socket_(0)
        , is_finish_(true)
    {

    }

    snapshot(uint64_t id, int pid, int socket, bool is_finish)
        : id_(id)
        , pid_(pid)
        , socket_(socket)
        , is_finish_(is_finish)
    {
    }

    uint64_t get_id()
    {
        return id_;
    }

    pid_t get_pid()
    {
        return pid_;
    }

    int get_socket()
    {
        return socket_;
    }

    bool is_finish()
    {
        return is_finish_;
    }

    void stop()
    {
        snapshot_event event = snapshot_event_stop;
        strong_write(socket_, reinterpret_cast<char*>(&event), sizeof(event));
        int status;
        waitpid(pid_, &status, WUNTRACED | WCONTINUED);
    }

    void start()
    {
        snapshot_event event = snapshot_event_start;
        strong_write(socket_, reinterpret_cast<char*>(&event), sizeof(event));
        int status;
        waitpid(pid_, &status, WUNTRACED | WCONTINUED);
    }

private:
    uint64_t id_;
    int pid_;
    int socket_;
    bool is_finish_;
};

}

#endif

