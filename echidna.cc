#include "echidna.hh"

#include <utility>

using namespace echidna;

Mutex::Mutex() : m_comm(MPI_COMM_NULL), m_commsz(0), m_thisrank(-1),
    m_wlwin(MPI_WIN_NULL), m_wltype(MPI_DATATYPE_NULL), m_wlsize(0),
    m_wldata(nullptr), m_wldata_private(nullptr)
{
    // no-op
}

Mutex::Mutex(MPI_Comm comm)
{
    // using a duplicated comm means the mutex doesn't need to tag messages
    // or worry about collisions between tag values from other parts of the code
    MPI_Comm_dup(comm, &m_comm);
    MPI_Comm_size(m_comm, &m_commsz);
    MPI_Comm_rank(m_comm, &m_thisrank);

    // NOTE: see Ross et al. (2005), Figure 3

    int blklens[2];
    blklens[0] = m_thisrank;
    blklens[1] = m_commsz - m_thisrank - 1;

    int disps[2];
    disps[0] = 0;
    disps[1] = m_thisrank + 1;

    MPI_Type_indexed(2, blklens, disps, MPI_BYTE, &m_wltype);
    MPI_Type_commit(&m_wltype);

    // NOTE: see Ross et al. (2005), Figure 1

    if (m_thisrank == m_homerank)
    {
        m_wlsize = m_commsz;
        m_wldata = new unsigned char[m_wlsize];
        for (int i = 0; i < m_wlsize; ++i) m_wldata[i] = 0;
    }
    else
    {
        m_wlsize = 0;
        m_wldata = nullptr;
    }

    MPI_Win_create(m_wldata, m_wlsize, 1, MPI_INFO_NULL, m_comm, &m_wlwin);

    m_wldata_private = new unsigned char[m_commsz-1];
}

Mutex::~Mutex()
{
    // check for null MPI objects before freeing
    if (m_wlwin != MPI_WIN_NULL) MPI_Win_free(&m_wlwin);
    if (m_wltype != MPI_DATATYPE_NULL) MPI_Type_free(&m_wltype);
    if (m_comm != MPI_COMM_NULL) MPI_Comm_free(&m_comm);

    // check for null data pointers before deleting
    if (m_wldata) delete[] m_wldata;
    if (m_wldata_private) delete[] m_wldata_private;
}

Mutex::Mutex(Mutex&& other)
{
    std::swap(m_comm, other.m_comm);
    std::swap(m_commsz, other.m_commsz);
    std::swap(m_thisrank, other.m_thisrank);

    std::swap(m_wlwin, other.m_wlwin);
    std::swap(m_wltype, other.m_wltype);

    std::swap(m_wlsize, other.m_wlsize);
    std::swap(m_wldata, other.m_wldata);
    std::swap(m_wldata_private, other.m_wldata_private);
}

Mutex& Mutex::operator=(Mutex&& other)
{
    std::swap(m_comm, other.m_comm);
    std::swap(m_commsz, other.m_commsz);
    std::swap(m_thisrank, other.m_thisrank);

    std::swap(m_wlwin, other.m_wlwin);
    std::swap(m_wltype, other.m_wltype);

    std::swap(m_wlsize, other.m_wlsize);
    std::swap(m_wldata, other.m_wldata);
    std::swap(m_wldata_private, other.m_wldata_private);

    return *this;
}

void Mutex::lock()
{
    // NOTE: see Ross et al. (2005), Figure 5

    unsigned char val = 1;

    // add this rank to the waitlist
    MPI_Win_lock(MPI_LOCK_EXCLUSIVE, m_homerank, 0, m_wlwin);
    MPI_Get(m_wldata_private, m_commsz - 1, MPI_BYTE,
        m_homerank, 0, 1, m_wltype, m_wlwin);
    MPI_Put(&val, 1, MPI_BYTE, m_homerank, m_thisrank,
        1, MPI_BYTE, m_wlwin);
    MPI_Win_unlock(m_homerank, m_wlwin);

    // check to see if the lock is held
    for (int i = 0; i < m_commsz - 1; ++i)
    {
        if (m_wldata_private[i] != 0)
        {
            // lock is held, wait for notification
            MPI_Recv(nullptr, 0, MPI_BYTE, MPI_ANY_SOURCE,
                MPI_ANY_TAG, m_comm, MPI_STATUS_IGNORE);
            break;
        }
    }

    // now we have the lock
}

void Mutex::unlock()
{
    // NOTE: see Ross et al. (2005), Figure 6

    unsigned char val = 0;

    // remove this rank from the waitlist
    MPI_Win_lock(MPI_LOCK_EXCLUSIVE, m_homerank, 0, m_wlwin);
    MPI_Get(m_wldata_private, m_commsz - 1, MPI_BYTE,
        m_homerank, 0, 1, m_wltype, m_wlwin);
    MPI_Put(&val, 1, MPI_BYTE, m_homerank, m_thisrank,
        1, MPI_BYTE, m_wlwin);
    MPI_Win_unlock(m_homerank, m_wlwin);

    for (int i = 0; i < m_commsz - 1; ++i)
    {
        if (m_wldata_private[i] != 0)
        {
            int nextrank = m_thisrank;

            // find the next rank waiting for the lock
            while ((nextrank < m_commsz - 1) &&
                   (m_wldata_private[nextrank] == 0))
            {
                nextrank += 1;
            }

            if (nextrank < m_commsz - 1) nextrank += 1;
            else
            {
                nextrank = 0;
                while ((nextrank < m_thisrank) &&
                       (m_wldata_private[nextrank] == 0))
                {
                    nextrank += 1;
                }
            }

            // notify the next rank with a zero-byte message
            MPI_Send(nullptr, 0, MPI_BYTE, nextrank, 0, m_comm);
            break;
        }
    }

    // now we have released the lock
}

// vim: set ft=cpp.doxygen:
