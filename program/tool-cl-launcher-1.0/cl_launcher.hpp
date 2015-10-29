// (c) 2015, dividiti
// BSD license

#ifndef CL_LAUNCHER_HPP
#define CL_LAUNCHER_HPP

#include <iostream>
#include <fstream>
#include <sstream>

#include <cstdlib>
#include <cassert>

#include <CL/cl.h>

#include <xopenme.h>

namespace gemmbench
{

class xopenme
{
private:
    static const int max_str_len = 1024;
    static const int max_tmr_count = 1;
    static const int max_var_count = 24;
    static const int max_work_dims = 3;

public:

    char str[max_str_len];
    int  var_count;

    cl_uint  tmp_uint;
    cl_ulong tmp_ulong;
    size_t   tmp_size_t;
    size_t   tmp_size_t_dims[max_work_dims];

    xopenme() :
        var_count(0),
        tmp_uint(0),
        tmp_ulong(0),
        tmp_size_t(0),
        tmp_size_t_dims() // C++11: {0, ..., 0}
    {
        assert(3 == max_work_dims);
        xopenme_init(max_tmr_count, max_var_count);
    }

    ~xopenme()
    {
        xopenme_dump_state();
    }

    bool var_count_below_max()
    {
        return var_count < max_var_count;
    }
};

class arguments
{
public:
    std::string file_name;
    cl_uint platform_idx;
    cl_uint device_idx;
    cl_uint matrix_order;

    arguments() :
        file_name(""),
        platform_idx(0),
        device_idx(0),
        matrix_order(512)
    { }

    void parse(int argc, char* argv[])
    {
        for (int i = 1; i < argc-1; ++i)
        {
            std::string this_arg(argv[i]);
            std::string next_arg(argv[++i]);

            if ("-f" == this_arg)
            {
                file_name = next_arg;
            }
            else if ("-p" == this_arg)
            {
                std::istringstream ss(next_arg); ss >> platform_idx;
            }
            else if ("-d" == this_arg)
            {
                std::istringstream ss(next_arg); ss >> device_idx;
            }
            else if ("-n" == this_arg)
            {
                std::istringstream ss(next_arg); ss >> matrix_order;
            }
            else
            {
                std::cerr << "Unhandled argument: " << this_arg << std::endl;
                --i; // skip only one argument
            }
        } // END OF for each argument

    } // END OF parse()

}; // END OF class arguments


class state
{
private:
    arguments args;
    xopenme openme;

public:
    cl_platform_id platform;
    cl_device_id device;
    cl_program program;

    state() :
        platform(NULL),
        device(NULL),
        program(NULL)
    { }

    ~state()
    { }

    void parse_arguments(int argc, char* argv[])
    {
        args.parse(argc, argv);

#if (1 == XOPENME)
        xopenme_add_var_s(openme.var_count++, (char*) "  \"CMD_LINE_ARGS#file_name\":\"%s\"", (char*) args.file_name.c_str());
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        xopenme_add_var_i(openme.var_count++, (char*) "  \"CMD_LINE_ARGS#platform_idx\":%u",  args.platform_idx);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        xopenme_add_var_i(openme.var_count++, (char*) "  \"CMD_LINE_ARGS#device_idx\":%u",    args.device_idx);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        xopenme_add_var_i(openme.var_count++, (char*) "  \"CMD_LINE_ARGS#matrix_order\":%u",  args.matrix_order);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");
#endif
    } // END OF parse_arguments()

    // Get OpenCL platform specified with the "-p" command line argument.
    void get_platform()
    {
        cl_uint err = CL_SUCCESS;

        cl_uint platform_idx = args.platform_idx;
        cl_uint num_entries = platform_idx+1;
        cl_platform_id * platforms = new cl_platform_id[num_entries];

        cl_uint num_platforms;
        err = clGetPlatformIDs(num_entries, platforms, &num_platforms);
        assert(CL_SUCCESS == err && "clGetPlatformIDs() failed.");
        assert(platform_idx < num_platforms && "No platform.");

        this->platform = platforms[platform_idx];
        delete [] platforms;

#if (1 == XOPENME)
        err = clGetPlatformInfo(platform, CL_PLATFORM_NAME,   sizeof(openme.str), &openme.str, NULL);
        assert(CL_SUCCESS == err && "clGetPlatformInfo() failed.");
        xopenme_add_var_s(openme.var_count++, (char*) "  \"CL_PLATFORM_NAME\":\"%s\"",    openme.str);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        err = clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, sizeof(openme.str), &openme.str, NULL);
        assert(CL_SUCCESS == err && "clGetPlatformInfo() failed.");
        xopenme_add_var_s(openme.var_count++, (char*) "  \"CL_PLATFORM_VENDOR\":\"%s\"",  openme.str);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        err = clGetPlatformInfo(platform, CL_PLATFORM_VERSION, sizeof(openme.str), &openme.str, NULL);
        assert(CL_SUCCESS == err && "clGetPlatformInfo() failed.");
        xopenme_add_var_s(openme.var_count++, (char*) "  \"CL_PLATFORM_VERSION\":\"%s\"", openme.str);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");
#endif
    } // END OF get_platform()


    // Get OpenCL device specified with the "-d" command line argument.
    void get_device()
    {
        cl_uint err = CL_SUCCESS;

        cl_uint device_idx = args.device_idx;
        cl_uint num_entries = device_idx+1;
        cl_device_id * devices = new cl_device_id[num_entries];

        cl_uint num_devices;
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, num_entries, devices, &num_devices);
        assert(CL_SUCCESS == err && "clGetDeviceIDs() failed.");
        assert(device_idx < num_devices && "No device.");

        this->device = devices[device_idx];
        delete [] devices;

#if (1 == XOPENME)
        err = clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(openme.str),   &openme.str, NULL);
        assert(CL_SUCCESS == err && "clGetDeviceInfo() failed.");
        xopenme_add_var_s(openme.var_count++, (char*) "  \"CL_DEVICE_NAME\":\"%s\"",   openme.str);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        err = clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(openme.str), &openme.str, NULL);
        assert(CL_SUCCESS == err && "clGetDeviceInfo() failed.");
        xopenme_add_var_s(openme.var_count++, (char*) "  \"CL_DEVICE_VENDOR\":\"%s\"", openme.str);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        err = clGetDeviceInfo(device, CL_DEVICE_VERSION, sizeof(openme.str), &openme.str, NULL);
        assert(CL_SUCCESS == err && "clGetDeviceInfo() failed.");
        xopenme_add_var_s(openme.var_count++, (char*) "  \"CL_DEVICE_VERSION\":\"%s\"", openme.str);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        err = clGetDeviceInfo(device, CL_DRIVER_VERSION, sizeof(openme.str), &openme.str, NULL);
        assert(CL_SUCCESS == err && "clGetDeviceInfo() failed.");
        xopenme_add_var_s(openme.var_count++, (char*) "  \"CL_DRIVER_VERSION\":\"%s\"", openme.str);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        err = clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &openme.tmp_uint, NULL);
        assert(CL_SUCCESS == err && "clGetDeviceInfo() failed.");
        xopenme_add_var_i(openme.var_count++, (char*) "  \"CL_DEVICE_MAX_COMPUTE_UNITS\":%u", openme.tmp_uint);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        err = clGetDeviceInfo(device, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), &openme.tmp_uint, NULL);
        assert(CL_SUCCESS == err && "clGetDeviceInfo() failed.");
        xopenme_add_var_i(openme.var_count++, (char*) "  \"CL_DEVICE_MAX_CLOCK_FREQUENCY\":%u", openme.tmp_uint);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        err = clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &openme.tmp_ulong, NULL);
        assert(CL_SUCCESS == err && "clGetDeviceInfo() failed.");
        xopenme_add_var_i(openme.var_count++, (char*) "  \"CL_DEVICE_GLOBAL_MEM_SIZE\":%u", openme.tmp_ulong);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        err = clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &openme.tmp_ulong, NULL);
        assert(CL_SUCCESS == err && "clGetDeviceInfo() failed.");
        xopenme_add_var_i(openme.var_count++, (char*) "  \"CL_DEVICE_LOCAL_MEM_SIZE\":%u", openme.tmp_ulong);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &openme.tmp_size_t, NULL);
        assert(CL_SUCCESS == err && "clGetDeviceInfo() failed.");
        xopenme_add_var_i(openme.var_count++, (char*) "  \"CL_DEVICE_MAX_WORK_GROUP_SIZE\":%u", openme.tmp_size_t);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(size_t), &openme.tmp_size_t, NULL);
        assert(CL_SUCCESS == err && "clGetDeviceInfo() failed.");
        xopenme_add_var_i(openme.var_count++, (char*) "  \"CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS\":%u", openme.tmp_size_t);
        assert(openme.tmp_size_t == 3 && "CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS != 3");
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(openme.tmp_size_t_dims), openme.tmp_size_t_dims, NULL);
        assert(CL_SUCCESS == err && "clGetDeviceInfo() failed.");
        xopenme_add_var_i(openme.var_count++, (char*) "  \"CL_DEVICE_MAX_WORK_ITEM_SIZES_0\":%u", openme.tmp_size_t_dims[0]);
        xopenme_add_var_i(openme.var_count++, (char*) "  \"CL_DEVICE_MAX_WORK_ITEM_SIZES_1\":%u", openme.tmp_size_t_dims[1]);
        xopenme_add_var_i(openme.var_count++, (char*) "  \"CL_DEVICE_MAX_WORK_ITEM_SIZES_2\":%u", openme.tmp_size_t_dims[2]);
#endif

    } // END OF get_device()


    // Get OpenCL program from file specified
    // with the "-f" command line argument.
    void get_program()
    {
        // Open file for reading. Close file on function exit.
        const std::string & file_name = args.file_name;
        std::cout << "Opening file \'" << file_name << "\' for reading..." << std::endl;
        std::ifstream file(file_name.c_str(), std::ifstream::binary);
        if (!file)
        {
            std::cerr << "Fatal error: cannot open file \'" << file_name << "\' for reading!" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Get file size.
        size_t file_size = 0;
        file.seekg(0, file.end);
        file_size = file.tellg();
        file.seekg(0, file.beg);

        // Read file into new buffer.
        char * file_data = new char[file_size];
        file.read(file_data, file_size);
        if (!file)
        {
            std::cerr << "Warning: only " << file.gcount() << " characters could be read out of " << file_size << std::endl;
        }

        // TODO: clCreateProgramWithSource() here.
        // https://www.khronos.org/registry/cl/sdk/1.1/docs/man/xhtml/clCreateProgramWithSource.html
        cl_uint err = CL_SUCCESS;

        delete [] file_data;

    } // END of get_program()

};


} // END OF namespace gemmbench

#endif // CL_LAUNCHER_HPP
