/*
 * Copyright (C) 1998-2014 ALPS Collaboration. See COPYRIGHT.TXT
 * All rights reserved. Use is subject to license terms. See LICENSE.TXT
 * For use in publications, see ACKNOWLEDGE.TXT
 */

#ifndef ALPS_NGS_STACKTRACE_HPP
#define ALPS_NGS_STACKTRACE_HPP

#include <alps/config.h>
#include <alps/utility/stringify.hpp>

#include <string>

// maximal number of stack frames displayed in stacktrace. Default 63
#ifndef ALPS_NGS_MAX_FRAMES
    #define ALPS_NGS_MAX_FRAMES 63
#endif

// prevent the signal object from registering signals
#ifdef BOOST_MSVC
    #define ALPS_NGS_NO_SIGNALS
#endif

// do not print a stacktrace in error messages
#ifndef __GNUC__
    #define ALPS_NGS_NO_STACKTRACE
#endif

// TODO: have_python
// TODO: have_mpi
// TODO: have_thread



// TODO: check for gcc and use __PRETTY_FUNCTION__

#define ALPS_STACKTRACE (                                                          \
      std::string("\nIn ") + __FILE__                                              \
    + " on " + ALPS_NGS_STRINGIFY(__LINE__)                                        \
    + " in " + __FUNCTION__ + "\n"                                          	   \
    + ::alps::ngs::stacktrace()                                                    \
)

namespace alps {
    namespace ngs {

        ALPS_DECL std::string stacktrace();

    }
}

#endif
