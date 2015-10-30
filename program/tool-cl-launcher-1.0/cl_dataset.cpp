// (c) 2015, dividiti
// BSD license

#include "cl_dataset.hpp"
#include "cl_state.hpp"

namespace gemmbench
{

// Compare the results against reference implementation.
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
template void dataset<cl_float>::verify_results(state& s, cl_float eps);
template void dataset<cl_double>::verify_results(state& s, cl_double eps);


} // END OF gemmbench namespace
