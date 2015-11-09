// (c) 2015, dividiti
// BSD license

#include "cl_dataset.hpp"
#include "cl_state.hpp"

#include <algorithm>

#include <cassert>
#include <cmath>

#include "xopenme.h"

namespace gemmbench
{

// Initialize the data in a random way.
template<typename T>
void dataset<T>::init_random(unsigned int seed, bool zero_matrix_C)
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
void dataset<T>::verify_results(state & s)
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
        const bool transA = s.meta.transA;
        const bool transB = s.meta.transB;
        const bool transC = false;

        // Compute reference.
        for (cl_uint i = 0; i < n; ++i)
        {
            for (cl_uint j = 0; j < n; ++j)
            {
                T ab = static_cast<T>(0);
                for (cl_uint k = 0; k < n; ++k)
                {
                    ab += matrix_A[index(i, k, transA)] * matrix_B[index(k, j, transB)];
                }
                matrix_C_ref[index(i, j, transC)] *= beta;
                matrix_C_ref[index(i, j, transC)] += alpha * ab;
            }
        }

        // Compare the results against reference.
        std::cout << "Comparing the OpenCL and reference results... ";
        const T eps = static_cast<T>(s.args.eps);
        T max_abs_diff = static_cast<T>(0);
        for (cl_uint i = 0; i < n; ++i)
        {
            for (cl_uint j = 0; j < n; ++j)
            {
                T abs_diff = abs(matrix_C[index(i, j, transC)] - matrix_C_ref[index(i, j, transC)]);
                max_abs_diff = std::max<T>(max_abs_diff, abs_diff);
                if (abs_diff > eps)
                {
                    std::cerr << "MISMATCH! (first at C[" << i << "][" << j << "])" << std::endl;
 #if (1 == XOPENME)
                    xopenme_add_var_i(s.openme.var_count++, (char*) "  \"RESULTS#match\":%u", 0);
                    assert(s.openme.var_count_below_max() && "xOpenME max var count reached.");
 #endif
                    exit(EXIT_FAILURE);
                }
            }
        }
        std::cout << "MATCH!" << std::endl;
#if (1 == XOPENME)
        xopenme_add_var_i(s.openme.var_count++, (char*) "  \"RESULTS#match\":%u", 1);
        assert(s.openme.var_count_below_max() && "xOpenME max var count reached.");

        xopenme_add_var_d(s.openme.var_count++, (char*) "  \"RESULTS#max_abs_diff\":%.8lf", max_abs_diff);
        assert(s.openme.var_count_below_max() && "xOpenME max var count reached.");
#endif

        delete [] matrix_C_ref;
    }

} // END OF dataset::verify_results()


// Instantiate templates to GEMM types.
template void dataset<cl_float>::init_random(unsigned int seed, bool zero_matrix_C);
template void dataset<cl_float>::verify_results(state& s);

template void dataset<cl_double>::init_random(unsigned int seed, bool zero_matrix_C);
template void dataset<cl_double>::verify_results(state& s);


} // END OF gemmbench namespace
