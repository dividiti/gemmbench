// (c) 2015, dividiti
// BSD license

#include "cl_state.hpp"
#include "cl_dataset.hpp"

static gemmbench::state state;

int main(int argc, char * argv[])
{
    std::cout << "Hello GEMMbench!" << std::endl;

    state.parse_arguments(argc, argv);
    
    state.get_platform();
    assert(state.platform && "No platform.");

    state.get_device();
    assert(state.device   && "No device."  );

    state.create_context();
    assert(state.context  && "No context." );

    state.create_queue();
    assert(state.queue    && "No queue."   );

    state.create_program();
    assert(state.program  && "No program." );

    state.build_program();
    assert(state.program  && "No program." );

    state.create_kernel();
    assert(state.kernel  &&  "No kernel." );

    gemmbench::dataset<cl_double> data(state.args.matrix_order);
    assert(data.matrix_A && "No data.");
    assert(data.matrix_B && "No data.");
    assert(data.matrix_C && "No data.");
    data.init_random();

    state.set_kernel_args<cl_double>(data);
    assert(state.buffer_A && "No buffer.");
    assert(state.buffer_B && "No buffer.");
    assert(state.buffer_C && "No buffer.");

    state.enqueue_kernel();

    state.profile_execution();

    data.verify_results(state);

    std::cout << "Bye GEMMbench!" << std::endl;

    exit(EXIT_SUCCESS);
}
