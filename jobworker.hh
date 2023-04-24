#ifndef ECHIDNA_JOBWORKER_HH
#define ECHIDNA_JOBWORKER_HH

#include <mpi.h>
#include <vector>
#include <optional>

namespace echidna
{

/**
 * @brief Communication framework for worker threads
 *
 * @rst
 * .. code-block:: cpp
 *    :caption: Example usage of job worker
 *
 *    MPI_Comm comm;
 *    JobWorker worker(comm);
 *
 *    while (true)
 *    {
 *        worker.notifyReadyThread();
 *        std::optional<std::vector<double>> task = worker.recvTask();
 *        if (!task) break;
 *
 *        // do something with (*task)
 *    }
 * @endrst
 */
struct JobWorker
{
    JobWorker(MPI_Comm comm);
    ~JobWorker();

    /**
     * @brief Notify the root thread that the worker is ready for a task
     */
    void notifyReadyThread() const;

    /**
     * @brief Gets the next task from the manager
     * @return A task, if one is available; otherwise, an empty result
     */
    std::optional<std::vector<double>> recvTask() const;

    private:
        MPI_Comm m_comm;
};

}   // namespace echidna

#endif      // ECHIDNA_JOBWORKER_HH

// vim: set ft=cpp.doxygen:
