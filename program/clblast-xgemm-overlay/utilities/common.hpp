
// =================================================================================================
// This file is part of the CLBlast project. The project is licensed under Apache Version 2.0. This
// project loosely follows the Google C++ styleguide and uses a tab-size of two spaces and a max-
// width of 100 characters per line.
//
// Author(s):
//   Cedric Nugteren <www.cedricnugteren.nl>
//
// This file contains all the interfaces to common kernels, such as copying, padding, and
// transposing a matrix. These functions are templated and thus header-only. This file also contains
// other common functions to routines, such as a function to launch a kernel.
//
// =================================================================================================

#ifndef CLBLAST_ROUTINES_COMMON_H_
#define CLBLAST_ROUTINES_COMMON_H_

#include <string>
#include <vector>

#include "clblast.h"
#include "clpp11.hpp"

namespace clblast {
// =================================================================================================

// Enqueues a kernel, waits for completion, and checks for errors
void RunKernel(Kernel &kernel, Queue &queue, const Device &device,
               std::vector<size_t> global, const std::vector<size_t> &local,
               EventPointer event, const std::vector<Event> &waitForEvents = {});

// =================================================================================================
} // namespace clblast

// CLBLAST_ROUTINES_COMMON_H_
#endif
