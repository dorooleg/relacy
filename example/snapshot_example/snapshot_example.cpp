#include "../../relacy/snapshots/snapshot_master.hpp"
#include "../../relacy/snapshots/snapshot_slave.hpp"
#include <iostream>
#include <cstdlib>
#include <vector>

struct ValueHolder
{
    ValueHolder(int& v) : v(v) { }
    void operator()() { ++v; }
    int& v;
};

int main()
{
    rl::snapshot_master master(5002);

    if (master.start() != 0)
    {
        std::cout << "Master" << std::endl;

        std::vector<rl::snapshot> snapshots;
        snapshots.push_back(master.wait_snapshot());
        while (!snapshots.back().is_finish())
        {
            snapshots.push_back(master.wait_snapshot());
        }

        snapshots.back().stop();
        snapshots.front().start();
        snapshots.clear();

        snapshots.push_back(master.wait_snapshot());
        while (!snapshots.back().is_finish())
        {
            snapshots.push_back(master.wait_snapshot());
        }

        for (std::vector<rl::snapshot>::iterator it = snapshots.begin(); it != snapshots.end(); ++it)
        {
            std::cout << it->get_id() << " ";
        }

        std::cout << std::endl;

        snapshots.back().stop();
        snapshots.front().start();
        snapshots.clear();

        snapshots.push_back(master.wait_snapshot());
        while (!snapshots.back().is_finish())
        {
            snapshots.push_back(master.wait_snapshot());
        }

        for (std::vector<rl::snapshot>::iterator it = snapshots.begin(); it != snapshots.end(); ++it)
        {
            std::cout << it->get_id() << " ";
        }

        std::cout << std::endl;


        exit(0);
    }

    int i = 5;

    rl::snapshot_slave::make_snapshot(5002, 1, ValueHolder(i));

    std::cout << "Slave " << i << std::endl;

    rl::snapshot_slave::make_snapshot(5002, 0, true);
}
