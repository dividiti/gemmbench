// (c) 2015, dividiti
// BSD license

#include "cl_launcher.hpp"

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
    assert(state.kernel  && "No kernel." );

    {
        const cl_float * matrix_A = NULL;
        const cl_float * matrix_B = NULL;
        cl_float * matrix_C = NULL;
        const cl_float alpha = 2.0f;
        const cl_float beta  = 4.0f;

        state.set_kernel_args(matrix_A, matrix_B, matrix_C,
            alpha, beta, state.args.matrix_order);
        assert(state.buffer_A && "No buffer.");
        assert(state.buffer_B && "No buffer.");
        assert(state.buffer_C && "No buffer.");
    }

    std::cout << "Bye GEMMbench!" << std::endl;

    exit(EXIT_SUCCESS);
}
