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
        , is_stop_(false)
    {

    }

    snapshot(uint64_t id, int pid, int socket, bool is_finish, bool is_stop = false)
        : id_(id)
        , pid_(pid)
        , socket_(socket)
        , is_finish_(is_finish)
        , is_stop_(is_stop)
    {
    }

    uint64_t get_id() const
    {
        return id_;
    }

    pid_t get_pid() const
    {
        return pid_;
    }

    int get_socket() const
    {
        return socket_;
    }

    bool is_finish() const
    {
        return is_finish_;
    }

    bool is_stop() const
    {
        return is_stop_;
    }

    void stop() const
    {
        snapshot_event event = snapshot_event_stop;
        strong_write(socket_, reinterpret_cast<char*>(&event), sizeof(event));
        int status;
        waitpid(pid_, &status, WCONTINUED | WNOHANG | WUNTRACED);
        close(socket_);
    }

    void start() const
    {
        snapshot_event event = snapshot_event_start;
        strong_write(socket_, reinterpret_cast<char*>(&event), sizeof(event));
        int status;
        waitpid(pid_, &status, WCONTINUED | WNOHANG | WUNTRACED);
        close(socket_);
    }

private:
    uint64_t id_;
    int      pid_;
    int      socket_;
    bool     is_finish_;
    bool     is_stop_;
};

}

#endif

