#include "jobmanager.hh"
#include "messages.hh"

using namespace echidna;

JobManager::JobManager(MPI_Comm comm)
{
    MPI_Comm_dup(comm, &m_comm);
}

JobManager::~JobManager()
{
    MPI_Comm_free(&m_comm);
}

int JobManager::waitForReadyThread() const
{
    MPI_Status status;
    MPI_Recv(nullptr, 0, MPI_CHAR, MPI_ANY_SOURCE, TAG_READY, m_comm, &status);

    return status.MPI_SOURCE;
}

void JobManager::sendTask(std::vector<double> const& task, int dest) const
{
    int msgsize = task.size();
    MPI_Send(task.data(), msgsize, MPI_DOUBLE, dest, TAG_TASK, m_comm);
}

void JobManager::done() const
{
    int size;
    MPI_Comm_size(m_comm, &size);

    for (int rnk = 0; rnk < size; ++rnk)
    {
        MPI_Send(nullptr, 0, MPI_CHAR, rnk, TAG_ALLDONE, m_comm);
    }
}

// vim: set ft=cpp.doxygen:
