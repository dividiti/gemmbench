// (c) 2015, dividiti
// BSD license

#include "cl_dataset.hpp"
#include "cl_state.hpp"

#include <cassert>
#include <cmath>

#include "xopenme.h"

namespace gemmbench
{

// Initialize the data in a random way.
template<typename T>
void dataset<T>::init_random()
{
    srand(seed);

    // Initialize the scalars.
    alpha = symmetric_rand();
    beta = symmetric_rand();

    // Initialize the matrices.
    for (cl_ulong i = 0, k = static_cast<cl_ulong>(n) * static_cast<cl_ulong>(n); i < k; ++i)
    {
        matrix_A[i] = symmetric_rand();
        matrix_B[i] = symmetric_rand();
        matrix_C[i] = zero_matrix_C ? static_cast<T>(0) : symmetric_rand();
    }

} // END OF init_random()


// Compare the results against a reference implementation.
template<typename T>
void dataset<T>::verify_results(state & s, T eps)
{
    // Read results from buffer_C.
    {
        assert(s.args.matrix_order == n && "Mismatching matrix orders.");

        cl_command_queue queue = s.queue;
        cl_mem buffer = s.buffer_C;
        cl_bool blocking_read = true;
        size_t offset = 0;
        size_t cb = sizeof(T) * n * n;
        void *ptr = matrix_C;
        cl_uint num_events_in_wait_list = 1;
        const cl_event event_wait_list[1] = { s.enqueue };
        cl_event *event = NULL;

        cl_int err = CL_SUCCESS;
        err = clEnqueueReadBuffer(queue, buffer, blocking_read,
            offset, cb, ptr, num_events_in_wait_list, event_wait_list, event);
        assert(CL_SUCCESS == err && "clEnqueueReadBuffer() failed.");
    }

    // Compute reference and compare the results against it.
    {
        T * matrix_C_ref = new T[n * n];

        // Compute reference.
        for (cl_uint i = 0; i < n; ++i)
        {
            for (cl_uint j = 0; j < n; ++j)
            {
                T ab = static_cast<T>(0);
                for (cl_uint k = 0; k < n; ++k)
                {
                    ab += matrix_A[index(i, k, false)] * matrix_B[index(k, j, true)];
                }
                matrix_C_ref[index(i, j, false)] *= beta;
                matrix_C_ref[index(i, j, false)] += alpha * ab;
            }
        }

        // Compare the results against reference.
        T max_abs_diff = static_cast<T>(0);
        for (cl_uint i = 0; i < n; ++i)
        {
            for (cl_uint j = 0; j < n; ++j)
            {
                T abs_diff = abs(matrix_C[index(i, j, false)] - matrix_C_ref[index(i, j, false)]);
                max_abs_diff = std::max<T>(max_abs_diff, abs_diff);
                if (abs_diff > eps)
                {
                    std::cerr << "The results and reference DO NOT match for C[" << i << "][" << j << "].";
                    std::cerr << "(No more mismatches will be reported.)" << std::endl;
 #if (1 == XOPENME)
                    xopenme_add_var_i(s.openme.var_count++, (char*) "  \"RESULTS#match\":%u", 0);
                    assert(s.openme.var_count_below_max() && "xOpenME max var count reached.");
 #endif
                    exit(EXIT_FAILURE);
                }
            }
        }
        std::cout << "The results and reference match." << std::endl;
#if (1 == XOPENME)
        xopenme_add_var_i(s.openme.var_count++, (char*) "  \"RESULTS#match\":%u", 1);
        assert(s.openme.var_count_below_max() && "xOpenME max var count reached.");

        xopenme_add_var_d(s.openme.var_count++, (char*) "  \"RESULTS#eps\":%.8lf", eps);
        assert(s.openme.var_count_below_max() && "xOpenME max var count reached.");

        xopenme_add_var_d(s.openme.var_count++, (char*) "  \"RESULTS#max_abs_diff\":%.8lf", max_abs_diff);
        assert(s.openme.var_count_below_max() && "xOpenME max var count reached.");
#endif

        delete [] matrix_C_ref;
    }

} // END OF dataset::verify_results()


// Instantiate templates to GEMM types.
template void dataset<cl_float>::init_random();
template void dataset<cl_float>::verify_results(state& s, cl_float eps);

template void dataset<cl_double>::init_random();
template void dataset<cl_double>::verify_results(state& s, cl_double eps);


} // END OF gemmbench namespace
