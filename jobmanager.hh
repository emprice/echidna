#ifndef ECHIDNA_JOBMANAGER_HH
#define ECHIDNA_JOBMANAGER_HH

#include "messages.hh"

#include <mpi.h>
#include <vector>

namespace echidna
{

/**
 * @brief Manager for parallel worker threads
 *
 * @rst
 * .. code-block:: cpp
 *    :caption: Example usage of job manager
 *
 *    MPI_Comm comm;
 *    JobManager manager(comm);
 *
 *    for (auto it = tasks.cbegin(); it != tasks.cend(); ++it)
 *    {
 *        int dest = manager.waitForReadyThread();
 *        manager.sendTask(task, dest);
 *    }
 *
 *    manager.done();
 * @endrst
 */
struct JobManager
{
    JobManager(MPI_Comm comm);
    ~JobManager();

    /**
     * @brief Waits to receive a ready message from a worker
     * @return The rank of the next ready thread
     */
    int waitForReadyThread() const;

    /**
     * @brief Sends a task to a single worker
     * @param[in] task Task to send, currently limited to a
     * vector of floats
     * @param[in] dest Rank where the task will be sent
     */
    void sendTask(std::vector<double> const& task, int dest) const;

    /**
     * @brief Alert the workers that all tasks are finished
     */
    void done() const;

    private:
        MPI_Comm m_comm;
};

}   // namespace echidna

#endif      // ECHIDNA_JOBMANAGER_HH

// vim: set ft=cpp.doxygen:
