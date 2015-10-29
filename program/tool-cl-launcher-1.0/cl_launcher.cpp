// (c) 2015, dividiti
// BSD license

#include "cl_launcher.hpp"

static gemmbench::state state;

int main(int argc, char * argv[])
{
    std::cout << "Hello GEMMbench!\n";

    state.parse_arguments(argc, argv);
    
    state.get_platform();
    assert(state.platform && "No platform.");

    state.get_device();
    assert(state.device && "No device.");

    state.get_program();
    //assert(state.program && "No program.");

    exit(EXIT_SUCCESS);
}
