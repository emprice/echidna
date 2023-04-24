/**
 * @file echidna.hh
 * @brief A simple API for an MPI-based mutex, heavily based on the
 * publication cited in the README.
 */

#ifndef ECHIDNA_MUTEX_HH
#define ECHIDNA_MUTEX_HH

#include <mpi.h>

namespace echidna
{

/**
 * @brief Simple mutex instance
 */
struct Mutex
{
    /// Default constructor; mutex will be in a safe, but invalid, state
    Mutex();

    /**
     * @brief Constructs a mutex for the given communicator
     * @param[in] comm Existing MPI communicator to duplicate
     */
    Mutex(MPI_Comm comm);

    /// Releases allocated resources for the mutex
    ~Mutex();

    /// Copy construction is explicitly disallowed
    Mutex(Mutex const& other) = delete;

    /// Copy assignment is explicitly disallowed
    Mutex& operator=(Mutex const& other) = delete;

    /// Move constructor; swaps an existing mutex with another
    Mutex(Mutex&& other);

    /// Move assignment; swaps an existing mutex with another
    Mutex& operator=(Mutex&& other);

    /**
     * @brief Blocks until the mutex can be locked, then returns once the
     * lock is held.
     */
    void lock();

    /**
     * @brief Unlocks the mutex so it can be locked by another rank
     */
    void unlock();

    private:
        MPI_Comm m_comm;    ///< Duplicated communicator
        int m_commsz;       ///< Size of `m_comm`
        int m_thisrank;     ///< Rank of this processor within `m_comm`

        MPI_Win m_wlwin;        ///< Window for waitlist

        int m_wlsize;                       ///< Size of waitlist on this rank
        unsigned char *m_wldata;            ///< Primary waitlist data
        unsigned char *m_wldata_private;    ///< Private copy of waitlist data

        /**
         * @brief Choose one rank to be the "home"; 0 is a good choice because
         * all MPI communicators will have a rank 0, but other choices are
         * possible here.
         */
        static constexpr int m_homerank = 0;
};

}   // namespace echidna

#endif      // ECHIDNA_MUTEX_HH

// vim: set ft=cpp.doxygen:
