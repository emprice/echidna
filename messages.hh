/**
 * @file messages.hh
 * @brief Definitions of MPI message tags and values
 */

#ifndef ECHIDNA_MESSAGES_HH
#define ECHIDNA_MESSAGES_HH

namespace echidna
{

enum MessageTags
{
    TAG_READY,      ///< Process is ready for another task
    TAG_TASK,       ///< Indicates that incoming data is a task
    TAG_ALLDONE,    ///< There are no more tasks
};

}

#endif      // ECHIDNA_MESSAGES_HH

// vim: set ft=cpp.doxygen:
