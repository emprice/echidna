#include "mutex.hh"
#include <iostream>

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    {
        MPI_Comm comm;
        MPI_Comm_dup(MPI_COMM_WORLD, &comm);

        int myrank;
        MPI_Comm_rank(comm, &myrank);

        auto mtx = echidna::Mutex(comm);
        mtx.lock();
        std::cout << myrank << " is the best rank!" << std::endl;
        mtx.unlock();

        MPI_Comm_free(&comm);
    }

    MPI_Finalize();

    return 0;
}

// vim: set ft=cpp.doxygen:
