// (c) 2015, dividiti
// BSD license

#include "cl_dataset.hpp"
#include "cl_state.hpp"

#include <cassert>

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
    }

    // TODO: Allocate memory for reference

} // END OF dataset::verify_results()


// Instantiate templates to GEMM types.
template void dataset<cl_float>::init_random();
template void dataset<cl_float>::verify_results(state& s, cl_float eps);

template void dataset<cl_double>::init_random();
template void dataset<cl_double>::verify_results(state& s, cl_double eps);


} // END OF gemmbench namespace
