// (c) 2015, dividiti
// BSD license

#ifndef CL_LAUNCHER_HPP
#define CL_LAUNCHER_HPP

#include <iostream>
#include <sstream>

#include <cstdlib>
#include <cassert>

#include <CL/cl.h>

#include <xopenme.h>

namespace gemmbench
{

class arguments
{
public:
    std::string file;
    cl_uint platform_idx;
    cl_uint device_idx;

    arguments() : file(""),
      platform_idx(0),
      device_idx(0)
    {}

    void parse(int argc, char* argv[])
    {
        for (int i = 1; i < argc-1; ++i)
        {
            std::string this_arg(argv[i]);
            std::string next_arg(argv[++i]);

            if ("-f" == this_arg)
            {
                file = next_arg;
            }
            else if ("-p" == this_arg)
            {
                std::istringstream ss(next_arg); ss >> platform_idx;
            } 
            else if ("-d" == this_arg)
            {
                std::istringstream ss(next_arg); ss >> device_idx;
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
    static const int xopenme_max_str_len = 1024;
    static const int xopenme_max_tmr_count = 1;
    static const int xopenme_max_var_count = 24;

    char xopenme_str[xopenme_max_str_len];
    int  xopenme_var_count;

    arguments args;

public:
    cl_platform_id platform;
    cl_device_id device;

    state() :
        xopenme_var_count(0),
        platform(NULL),
        device(NULL)
    {
        xopenme_init(xopenme_max_tmr_count, xopenme_max_var_count);
    }

    ~state()
    {
        xopenme_dump_state();
    }

    void parse_arguments(int argc, char* argv[])
    {
        args.parse(argc, argv);
    }

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
        err = clGetPlatformInfo(platform, CL_PLATFORM_NAME,   sizeof(xopenme_str), &xopenme_str, NULL);
        assert(CL_SUCCESS == err && "clGetPlatformInfo() failed.");
        xopenme_add_var_s(xopenme_var_count++, (char*) "  \"CL_PLATFORM_NAME\":\"%s\"",   xopenme_str);
        assert(xopenme_var_count < xopenme_max_var_count && "xOpenME max var count reached.");

        err = clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, sizeof(xopenme_str), &xopenme_str, NULL);
        assert(CL_SUCCESS == err && "clGetPlatformInfo() failed.");
        xopenme_add_var_s(xopenme_var_count++, (char*) "  \"CL_PLATFORM_VENDOR\":\"%s\"", xopenme_str);
        assert(xopenme_var_count < xopenme_max_var_count && "xOpenME max var count reached.");

        err = clGetPlatformInfo(platform, CL_PLATFORM_VERSION, sizeof(xopenme_str), &xopenme_str, NULL);
        assert(CL_SUCCESS == err && "clGetPlatformInfo() failed.");
        xopenme_add_var_s(xopenme_var_count++, (char*) "  \"CL_PLATFORM_VERSION\":\"%s\"", xopenme_str);
        assert(xopenme_var_count < xopenme_max_var_count && "xOpenME max var count reached.");
#endif
    } // END OF get_platform()


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
        err = clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(xopenme_str),   &xopenme_str, NULL);
        assert(CL_SUCCESS == err && "clGetDeviceInfo() failed.");
        xopenme_add_var_s(xopenme_var_count++, (char*) "  \"CL_DEVICE_NAME\":\"%s\"",   xopenme_str);
        assert(xopenme_var_count < xopenme_max_var_count && "xOpenME max var count reached.");

        err = clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(xopenme_str), &xopenme_str, NULL);
        assert(CL_SUCCESS == err && "clGetDeviceInfo() failed.");
        xopenme_add_var_s(xopenme_var_count++, (char*) "  \"CL_DEVICE_VENDOR\":\"%s\"", xopenme_str);
        assert(xopenme_var_count < xopenme_max_var_count && "xOpenME max var count reached.");

        err = clGetDeviceInfo(device, CL_DEVICE_VERSION, sizeof(xopenme_str), &xopenme_str, NULL);
        assert(CL_SUCCESS == err && "clGetDeviceInfo() failed.");
        xopenme_add_var_s(xopenme_var_count++, (char*) "  \"CL_DEVICE_VERSION\":\"%s\"", xopenme_str);
        assert(xopenme_var_count < xopenme_max_var_count && "xOpenME max var count reached.");
#endif
    } // END OF get_device()

};


} // END OF namespace gemmbench

#endif // CL_LAUNCHER_HPP
