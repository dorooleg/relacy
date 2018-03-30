#ifndef RL_RANDOM_SNAPSHOTS_SCHEDULER_HPP
#define RL_RANDOM_SNAPSHOTS_SCHEDULER_HPP
#ifdef _MSC_VER
#   pragma once
#endif

#include "../base.hpp"
#include "scheduler.hpp"
#include "../random.hpp"
#include "../snapshots/snapshot_master.hpp"
#include "../snapshots/snapshot_slave.hpp"

namespace rl
{

class random_snapshots_scheduler : public scheduler<random_snapshots_scheduler, scheduler_thread_info>
{
public:
    typedef scheduler<random_snapshots_scheduler, scheduler_thread_info> base_t;
    typedef typename base_t::thread_info_t thread_info_t;
    typedef typename base_t::shared_context_t shared_context_t;

    struct task_t
    {
    };

private:
    template<typename T>
    class incrementor_functor
    {
    public:
        incrementor_functor(T& value, random_generator& rand)
            : value_(value)
            , rand_(rand)
        {
        }

        void operator()()
        {
            ++value_;
            rand_.seed(value_);
        }

    private:
        T& value_;
        random_generator& rand_;
    };

public:
    random_snapshots_scheduler(test_params& params, shared_context_t& ctx, thread_id_t dynamic_thread_count, thread_id_t thread_count)
        : base_t(params, ctx, dynamic_thread_count, thread_count)
        , is_finish_(true)
    {
        rl::snapshot_master master(5001);

        if (master.start() != 0)
        {
            std::vector<snapshot> snapshots;
            for (iteration_t i = 1; i < this->params_.iteration_count; i++)
            {
                snapshots.push_back(master.wait_snapshot());
                while (!snapshots.back().is_finish())
                {
                    snapshots.push_back(master.wait_snapshot());
                }

                int split = rand_.rand() % snapshots.size();

                snapshot current_snapshot = snapshots[split];

                for (std::vector<snapshot>::iterator it = snapshots.begin() + split + 1; it != snapshots.end(); ++it)
                {
                    it->stop();
                }

                snapshots.resize(split);

                current_snapshot.start();
            }

            for (std::vector<snapshot>::iterator it = snapshots.begin(); it != snapshots.end(); ++it)
            {
                it->stop();
            }

            this->iter_ = this->params_.iteration_count == 0 ? 0 : this->params_.iteration_count - 1;
            rand_.seed(this->iter_);

            is_finish_ = false;
        }
    }

    thread_id_t iteration_begin_impl()
    {
        rand_.seed(this->iter_);
        unpark_reason reason;
        return schedule_impl(reason, false);
    }

    bool iteration_end_impl()
    {
        rl::snapshot_slave::make_snapshot(5001, 0, is_finish_);
        return !is_finish_ || this->iter_ == this->params_.iteration_count;
    }

    thread_id_t schedule_impl(unpark_reason& reason, unsigned /*yield*/)
    {
        rl::snapshot_slave::make_snapshot(5001, 0, incrementor_functor<iteration_t>(this->iter_, rand_));

        thread_id_t const running_thread_count = this->running_threads_count;

        thread_id_t timed_thread_count = this->timed_thread_count_;
        if (timed_thread_count)
        {
            thread_id_t cnt = running_thread_count ? timed_thread_count * 4 : timed_thread_count;
            thread_id_t idx = rand_.rand() % cnt;
            if (idx < timed_thread_count)
            {
                thread_info_t* thr = this->timed_threads_[idx];
                thread_id_t th = thr->index_;
                RL_VERIFY(1 == thr->block_count_);
                this->unpark_thread(th);
                RL_VERIFY(thr->state_ == thread_state_running);
                reason = unpark_reason_timeout;
                return th;
            }
        }

        thread_id_t spurious_thread_count = this->spurious_thread_count_;
        if (spurious_thread_count && running_thread_count)
        {
            thread_id_t cnt = spurious_thread_count * 8;
            thread_id_t idx = rand_.rand() % cnt;
            if (idx < spurious_thread_count)
            {
                thread_info_t* thr = this->spurious_threads_[idx];
                thread_id_t th = thr->index_;
                RL_VERIFY(1 == thr->block_count_);
                this->unpark_thread(th);
                RL_VERIFY(thr->state_ == thread_state_running);
                reason = unpark_reason_spurious;
                return th;
            }
        }

        RL_VERIFY(running_thread_count);
        unsigned index = rand_.rand() % running_thread_count;
        thread_id_t th = this->running_threads[index];
        reason = unpark_reason_normal;
        return th;
    }

    unsigned rand_impl(unsigned limit, sched_type t)
    {
        (void)t;
        return rand_.rand() % limit;
    }

    iteration_t iteration_count_impl()
    {
        return this->params_.iteration_count;
    }

    void get_state_impl(std::ostream& /*ss*/)
    {
    }

    void set_state_impl(std::istream& /*ss*/)
    {
    }

    void on_thread_block(thread_id_t /*th*/, bool /*yield*/)
    {
    }

private:
    random_generator rand_;
    bool is_finish_;

    RL_NOCOPY(random_snapshots_scheduler);
};


}

#endif

