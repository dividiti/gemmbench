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

    std::cout << "Bye GEMMbench!" << std::endl;

    exit(EXIT_SUCCESS);
}
