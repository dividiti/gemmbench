// (c) 2015, dividiti
// BSD license

#include "cl_state.hpp"

#include <fstream>
#include <iostream>

namespace gemmbench
{


bool
file::read(const std::string & path, buffer & buff)
{
    bool success = true;

    // Open file for reading. Close file on function exit.
    std::ifstream file(path.c_str(), std::ifstream::binary);
    if (!file)
    {
        std::cerr << "Fatal error: cannot open file \'" << path << "\' for reading!" << std::endl;
        success = false;
    }
    else
    {
        // Get file size.
        file.seekg(0, file.end);
        buff.size = file.tellg();
        file.seekg(0, file.beg);

        // Read file into new buffer.
        buff.data = new char[buff.size];
        file.read(buff.data, buff.size);
        if (!file)
        {
            std::cerr << "Warning: only " << file.gcount() << " characters could be read out of " << buff.size << std::endl;
            success = false;
        }
    }

    return success;

} // END OF file::read()


bool
file::write(const std::string & path, const buffer & buff)
{
    bool success = true;

    // Open file for writing. Close file on function exit.
    std::ofstream file(path.c_str(), std::ofstream::binary);
    if (!file)
    {
        std::cerr << "Fatal error: cannot open file \'" << path << "\' for writing!" << std::endl;
        success = false;
    }
    else
    {
        file.write(buff.data, buff.size);
        // TODO: check success.
    }

    return success;

} // END OF file::write()


} // END OF gemmbench namespace
