#include "jobworker.hh"
#include "messages.hh"

#include <stdexcept>

using namespace echidna;

JobWorker::JobWorker(MPI_Comm comm)
{
    MPI_Comm_dup(comm, &m_comm);
}

JobWorker::~JobWorker()
{
    MPI_Comm_free(&m_comm);
}

void JobWorker::notifyReadyThread() const
{
    MPI_Send(nullptr, 0, MPI_CHAR, 0 /* dest rank */, TAG_READY, m_comm);
}

std::optional<std::vector<double>> JobWorker::recvTask() const
{
    MPI_Status status;

    // we don't know which message to expect, so probe for any message
    MPI_Probe(0 /* source rank */, MPI_ANY_TAG, m_comm, &status);

    switch (status.MPI_TAG)
    {
        case TAG_TASK:
        {
            std::vector<double> task;

            int msgsize;
            MPI_Get_count(&status, MPI_DOUBLE, &msgsize);
            task.resize(msgsize);

            MPI_Recv(task.data(), msgsize, MPI_DOUBLE,
                status.MPI_SOURCE, status.MPI_TAG, m_comm, &status);

            return std::make_optional(task);
        }

        case TAG_ALLDONE:
            return std::optional<std::vector<double>>();

        default:
            throw std::runtime_error("unrecognized tag");
    }
}

// vim: set ft=cpp.doxygen:
