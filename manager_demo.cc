#include "mutex.hh"
#include "jobworker.hh"
#include "jobmanager.hh"
#include <iostream>

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    {
        MPI_Comm comm;
        MPI_Comm_dup(MPI_COMM_WORLD, &comm);

        int myrank, worldsize;
        MPI_Comm_rank(comm, &myrank);
        MPI_Comm_size(comm, &worldsize);

        auto mtx = echidna::Mutex(comm);

        if (worldsize == 1)
        {
            std::cout << "This demo requires more than one process" << std::endl;
        }
        else
        {
            if (!myrank)
            {
                auto manager = echidna::JobManager(comm);

                for (size_t n = 0; n < 100; ++n)
                {
                    std::vector<double> task;
                    task.push_back(n);

                    int dest = manager.waitForReadyThread();
                    manager.sendTask(task, dest);
                }

                manager.done();
            }
            else
            {
                auto worker = echidna::JobWorker(comm);

                while (true)
                {
                    worker.notifyReadyThread();
                    std::optional<std::vector<double>> task = worker.recvTask();
                    if (!task) break;

                    mtx.lock();
                    std::cout << myrank << " got task data "
                              << (*task)[0] << "!" << std::endl;
                    mtx.unlock();
                }
            }
        }

        MPI_Comm_free(&comm);
    }

    MPI_Finalize();

    return 0;
}

// vim: set ft=cpp.doxygen:
