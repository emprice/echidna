echidna
=======

*echidna: Half-woman, half-snake, and mother of the Lernaean Hydra*

This repository exposes a simple C++ implementation of an MPI mutex based
on the publication below. Full credit for this idea, the algorithm, and
the example implementation in the paper go to those authors. Only the
specific API and the CMake configuration were written by me.

```bib
@inproceedings{Ross+2005,
    author={Ross, R. and Latham, R. and Gropp, W. and Thakur, R. and Toonen, B.},
 booktitle={CCGrid 2005. IEEE International Symposium on Cluster Computing and the Grid, 2005.},
     title={Implementing MPI-IO atomic mode without file system support},
      year={2005},
    volume={2},
     pages={1135--1142},
       doi={10.1109/CCGRID.2005.1558687}
}
```

To include *echidna* into a larger project, simply clone it as a submodle
with the command below.

```sh
git submodule add https://github.com/emprice/echidna.git
cmake -DMPI_ROOT=/path/to/mpi -G Ninja -B build .
```

Write some code that uses the interface. A minimal working example is
given below.

```cpp
#include <echidna.hh>
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
```

In the `CMakeLists.txt` for the bigger project, add the submodule
directory and link as you would any other library.

```cmake
add_subdirectory(echidna)
add_executable(hello hello.cc)
target_link_libraries(hello PRIVATE echidna)
```

<!-- vim: set ft=markdown: -->
