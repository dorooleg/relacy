#include "../../relacy/snapshots/snapshot_master.hpp"
#include "../../relacy/snapshots/snapshot_slave.hpp"
#include <iostream>
#include <cstdlib>
#include <vector>

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
        snapshots.resize(1);
        snapshots.front().start();

        exit(0);
    }

    rl::snapshot_slave::make_snapshot(5002, 1);

    std::cout << "Slave" << std::endl;

    rl::snapshot_slave::make_snapshot(5002, 0, true);
}
