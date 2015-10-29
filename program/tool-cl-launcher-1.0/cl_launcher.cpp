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

    exit(EXIT_SUCCESS);
}
