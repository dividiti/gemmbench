// (c) 2015, dividiti
// BSD license

#ifndef CL_STATE_HPP
#define CL_STATE_HPP

#include "cl_dataset.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include <cstdlib>
#include <cassert>

#include <CL/cl.h>

#include <xopenme.h>
#include <cJSON.h>

namespace gemmbench
{

template<typename T> class dataset;


class file
{
public:
    struct buffer
    {
        char * data;
        size_t size;

        // Default constructor.
        buffer() : data(NULL), size(0) { }

        // Initializing constructor.
        buffer(char * _data, size_t _size) :
            data(_data), size(_size) { }

        // Initializing constructor with pointer cast.
        buffer(const char * _data, size_t _size) :
            data((char*) _data), size(_size) { }

        // Destructor.
        ~buffer() { delete [] data; }

    }; // END OF buffer struct

    // Read file into buffer. Return true on success.
    static bool read(const std::string & path, buffer & buff);

    // Write file from buffer. Return true on success.
    static bool write(const std::string & path, const buffer & buff);

    // Prevent instantiating this class.
    virtual ~file();

}; // END OF file class


class xopenme
{
private:
    static const int max_str_len = 1000;
    static const int max_var_count = 50;
    static const int max_tmr_count = 1;
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

}; // END OF gemmbench::xopenme class


class arguments
{
private:
    void print_usage(const char * cmd)
    {
        // Print on one line.
        std::cout << "Usage: " << cmd ;
        std::cout << " -f <file name>";
        std::cout << " -p <platform index>";
        std::cout << " -d <device index>";
        std::cout << " -n <matrix order>";
        std::cout << std::endl;

        exit(EXIT_SUCCESS);
    }

public:
    std::string file_name;
    cl_uint platform_idx;
    cl_uint device_idx;
    cl_uint matrix_order;

    arguments() :
        file_name(""),
        platform_idx(0),
        device_idx(0),
        matrix_order(256)
    { }

    void parse(int argc, char* argv[])
    {
        if (1 == argc)
        {
            print_usage(argv[0]);
        }

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

}; // END OF gemmbench::arguments class


class metadata
{
public:
    // Program name.
    std::string name;

    // Program file name.
    std::string file;

    // Program build options.
    std::string opts;

    // Single precision real ("S") and complex ("C").
    // Double precision real ("G") and complex ("Z").
    std::string type;

    // Transposed ('T'), if true; non-transposed ('N'), if false.
    bool transA;
    bool transB;

    // Coarsening factors along rows ("di") and columns ("dj").
    cl_uint di;
    cl_uint dj;

    // Constructor.
    metadata() :
        name(""),
        file(""),
        opts(""),
        type(""),
        transA(false),
        transB(false),
        di(1),
        dj(1)
    { }

    // Destructor.
    ~metadata()
    { }

    // Parse the metadata file specified with the "-f" command line argument.
    // Example file:
    //   {
    //     "name"   : "DGEMM_NT_1x1",
    //     "file"   : "DGEMM_NT_1x1.cl",
    //     "type"   : "D",
    //     "transA" : "N",
    //     "transB" : "T",
    //     "dj"     : 1,
    //     "di"     : 1
    //   }
    void parse(const std::string & path)
    {
#if (1 == TEST_METADATA)
        std::cout << "Reading OpenCL kernel metadata from string:" << std::endl;
        const char *raw_meta = "{\n"
            "  \"name\"   : \"DGEMM_NT_1x1\",\n"
            "  \"file\"   : \"DGEMM_NT_1x1.cl\",\n"
            "  \"type\"   : \"D\",\n"
            "  \"transB\" : \"T\"\n"
            "}\n";
#else
        std::cout << "Reading OpenCL kernel metadata from \'" << path << "\'..." << std::endl;
        file::buffer buffer;
        bool read_success = file::read(path, buffer);
        assert(read_success && "file::read() failed.");
        assert(buffer.data && buffer.size > 0 && "No metadata.");
        buffer.data[buffer.size-1] = '\0'; // null-terminate
        const char * raw_meta = buffer.data;
#endif
        std::cout << raw_meta << std::endl;

        cJSON* meta = cJSON_Parse(raw_meta);
        for (int i = 0; i < cJSON_GetArraySize(meta); ++i)
        {
            const cJSON *meta_i = cJSON_GetArrayItem(meta, i);
            const int value_type = meta_i->type;
            const std::string key(meta_i->string);
            if ("name" == key)
            {
                assert(cJSON_String == value_type && "Unexpected value type.");
                name = std::string(meta_i->valuestring);
            }
            else if ("file" == key)
            {
                assert(cJSON_String == value_type && "Unexpected value type.");
                file = std::string(meta_i->valuestring);
            }
            else if ("opts" == key)
            {
                assert(cJSON_String == value_type && "Unexpected value type.");
                opts = std::string(meta_i->valuestring);
            }
            else if ("type" == key)
            {
                assert(cJSON_String == value_type && "Unexpected value type.");
                type = std::string(meta_i->valuestring);
                assert(type == "S" || type == "C" || type == "D" || type == "Z");
            }
            else if ("transA" == key)
            {
                assert(cJSON_String == value_type && "Unexpected value type.");
                transA = std::string(meta_i->valuestring) == "T" ? true : false;
            }
            else if ("transB" == key)
            {
                assert(cJSON_String == value_type && "Unexpected value type.");
                transB = std::string(meta_i->valuestring) == "T" ? true : false;
            }
            else if ("di" == key)
            {
                assert(cJSON_Number == value_type && "Unexpected value type.");
                di = meta_i->valueint;
            }
            else if ("dj" == key)
            {
                assert(cJSON_Number == value_type && "Unexpected value type.");
                dj = meta_i->valueint;
            }
            else
            {
                std::cerr << "Unhandled key: " << key << std::endl;
            }
        } // END OF for each key:value pair
        cJSON_Delete(meta);

    } // END OF parse()

}; // END OF gemmbench::metadata class


class state
{
public:
    // Command line arguments (with defaults).
    arguments args;

    // Kernel metadata (with defaults).
    metadata meta;

    // xOpenME state.
    xopenme openme;

    // OpenCL objects.
    cl_platform_id   platform;
    cl_device_id     device;
    cl_context       context;
    cl_command_queue queue;
    cl_program       program;
    cl_kernel        kernel;
    cl_mem           buffer_A;
    cl_mem           buffer_B;
    cl_mem           buffer_C;
    cl_event         enqueue;

    // Constructor.
    state() :
        platform(NULL),
        device(NULL),
        context(NULL),
        queue(NULL),
        program(NULL),
        kernel(NULL),
        buffer_A(NULL),
        buffer_B(NULL),
        buffer_C(NULL),
        enqueue(NULL)
    { }

    // Destructor. Release in reverse order.
    ~state()
    {
        cl_int err = CL_SUCCESS;

        err = clReleaseEvent(enqueue);
        assert(CL_SUCCESS == err && "clReleaseEvent() failed.");

        err = clReleaseMemObject(buffer_C);
        assert(CL_SUCCESS == err && "clReleaseMemObject() failed.");

        err = clReleaseMemObject(buffer_B);
        assert(CL_SUCCESS == err && "clReleaseMemObject() failed.");

        err = clReleaseMemObject(buffer_A);
        assert(CL_SUCCESS == err && "clReleaseMemObject() failed.");

        err = clReleaseKernel(kernel);
        assert(CL_SUCCESS == err && "clReleaseKernel() failed.");

        err = clReleaseProgram(program);
        assert(CL_SUCCESS == err && "clReleaseProgram() failed.");

        err = clReleaseCommandQueue(queue);
        assert(CL_SUCCESS == err && "clReleaseCommandQueue() failed.");

        err = clReleaseContext(context);
        assert(CL_SUCCESS == err && "clReleaseContext() failed.");
    }

    void init(int argc, char* argv[])
    {
        // Parse command line arguments.
        args.parse(argc, argv);

#if (1 == XOPENME)
        xopenme_add_var_s(openme.var_count++,
            (char*) "  \"CMD_LINE_ARGS#file_name\":\"%s\"", (char*) args.file_name.c_str());
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        xopenme_add_var_i(openme.var_count++,
            (char*) "  \"CMD_LINE_ARGS#platform_idx\":%u",  args.platform_idx);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        xopenme_add_var_i(openme.var_count++,
            (char*) "  \"CMD_LINE_ARGS#device_idx\":%u",    args.device_idx);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        xopenme_add_var_i(openme.var_count++,
            (char*) "  \"CMD_LINE_ARGS#matrix_order\":%u",  args.matrix_order);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");
#endif

        // Parse kernel metadata.
        meta.parse(args.file_name);

#if (1 == XOPENME)
        xopenme_add_var_s(openme.var_count++,
            (char*) "  \"METADATA#name\":\"%s\"", (char*) meta.name.c_str());
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        xopenme_add_var_s(openme.var_count++,
            (char*) "  \"METADATA#file\":\"%s\"", (char*) meta.file.c_str());
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        xopenme_add_var_s(openme.var_count++,
            (char*) "  \"METADATA#opts\":\"%s\"", (char*) meta.opts.c_str());
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        xopenme_add_var_s(openme.var_count++,
            (char*) "  \"METADATA#type\":\"%s\"", (char*) meta.type.c_str());
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        xopenme_add_var_i(openme.var_count++,
            (char*) "  \"METADATA#transA\":%u",    (int) meta.transA);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        xopenme_add_var_i(openme.var_count++,
            (char*) "  \"METADATA#transB\":%u",    (int) meta.transB);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        xopenme_add_var_i(openme.var_count++,
            (char*) "  \"METADATA#di\":%u",    (int) meta.di);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        xopenme_add_var_i(openme.var_count++,
            (char*) "  \"METADATA#dj\":%u",    (int) meta.dj);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");
#endif

    } // END OF parse_arguments()


    // Get OpenCL platform specified with the "-p" command line argument.
    void get_platform()
    {
        cl_int err = CL_SUCCESS;

        cl_uint platform_idx = args.platform_idx;
        cl_uint num_entries = platform_idx+1;
        cl_platform_id * platforms = new cl_platform_id[num_entries];

        cl_uint num_platforms;
        err = clGetPlatformIDs(num_entries, platforms, &num_platforms);
        assert(CL_SUCCESS == err && "clGetPlatformIDs() failed.");
        assert(platform_idx < num_platforms && "Platform not available.");

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
        cl_int err = CL_SUCCESS;

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


    // Create OpenCL context for platform and device.
    void create_context()
    {
        cl_context_properties properties[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties) platform, 0 };
        cl_int err = CL_SUCCESS;
        context = clCreateContext(properties, /* number of devices */ 1, &device,
            /* callback fn */ NULL, /* callback data */ NULL, &err);
        assert(CL_SUCCESS == err && "clCreateContext() failed.");
    } // END OF create_context()


    // Create OpenCL queue for context and device with profiling enabled.
    void create_queue()
    {
        cl_command_queue_properties properties = CL_QUEUE_PROFILING_ENABLE;
        cl_int err = CL_SUCCESS;
        queue = clCreateCommandQueue(context, device, properties, &err);
        assert(CL_SUCCESS == err && "clCreateCommandQueue() failed.");
    } // END OF create_queue()


    // Create OpenCL program from file specified in the metadata supplied with the "-f" command line argument.
    void create_program()
    {
        //TODO: Read the name of the file containing an OpenCL program from the metadata file.
        //FIXME: For now, just assume the OpenCL file thas the same base name and the ".cl" extension.
        const size_t index_of_last_dot = args.file_name.find_last_of(".");
        const std::string file_name = args.file_name.substr(0, index_of_last_dot).append(".cl");
        std::cout << "Reading an OpenCL kernel from \'" << file_name << "\'..." << std::endl;

        // Open file for reading. Close file on function exit.
        file::buffer buffer;
        bool read_success = file::read(file_name, buffer);
        assert(read_success && "file::read() failed.");

        // Create program from source.
        cl_int err = CL_SUCCESS;
        program = clCreateProgramWithSource(context,
            /* num_strings */ 1, /* strings */ (const char**) &buffer.data, /* lengths */ &buffer.size, &err);
        assert(CL_SUCCESS == err && "clCreateProgramWithSource() failed.");

    } // END OF create_program()


    // Build program with options specified with the "-b" command line argument.
    void build_program()
    {
        cl_int err = CL_SUCCESS;
        const char * build_options = meta.opts.c_str();
        err = clBuildProgram(program, /* num_devices */ 1, /* device_list */ &device,
            /* options */ build_options, /* callback fn */ NULL, /* callback data */ NULL);
        assert(CL_SUCCESS == err && "clBuildProgram() failed.");
    } // END OF build_program()


    // Create kernel called "gemm".
    void create_kernel()
    {
        cl_int err = CL_SUCCESS;
        const char * kernel_name = "gemm";
        kernel = clCreateKernel(program, kernel_name, &err);
        assert(CL_SUCCESS == err && "clCreateKernel() failed.");
    } // END OF create_kernel()

private:
    // Create buffers and set kernel arguments.
    template<typename T> void set_kernel_args(const dataset<T>& data)
    {
        cl_int err = CL_SUCCESS;

        // Create buffers.
        buffer_A = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            data.n * data.n * sizeof(T), (T*) data.matrix_A, &err);
        assert(CL_SUCCESS == err && "clCreateBuffer() failed.");

        buffer_B = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            data.n * data.n * sizeof(T), (T*) data.matrix_B, &err);
        assert(CL_SUCCESS == err && "clCreateBuffer() failed.");

        buffer_C = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
            data.n * data.n * sizeof(T), (T*) data.matrix_C, &err);
        assert(CL_SUCCESS == err && "clCreateBuffer() failed.");

        // Set kernel arguments.
        cl_uint arg_count = 0;

        err = clSetKernelArg(kernel, arg_count++, sizeof(cl_mem), &buffer_A);
        assert(CL_SUCCESS == err && "clSetKernelArg() failed.");

        err = clSetKernelArg(kernel, arg_count++, sizeof(cl_mem), &buffer_B);
        assert(CL_SUCCESS == err && "clSetKernelArg() failed.");

        err = clSetKernelArg(kernel, arg_count++, sizeof(cl_mem), &buffer_C);
        assert(CL_SUCCESS == err && "clSetKernelArg() failed.");

        err = clSetKernelArg(kernel, arg_count++, sizeof(T), &data.alpha);
        assert(CL_SUCCESS == err && "clSetKernelArg() failed.");

        err = clSetKernelArg(kernel, arg_count++, sizeof(T), &data.beta);
        assert(CL_SUCCESS == err && "clSetKernelArg() failed.");

        err = clSetKernelArg(kernel, arg_count++, sizeof(cl_uint), &data.n);
        assert(CL_SUCCESS == err && "clSetKernelArg() failed.");
    }


    void enqueue_kernel()
    {
        cl_int err = CL_SUCCESS;

        // Enqueue kernel.
        {
            const size_t n = (size_t) args.matrix_order;
            const size_t work_dim = 2; assert(0 < work_dim && work_dim <= 3);
            const size_t global_work_offset[work_dim] = { 0, 0 };
            const size_t global_work_size[work_dim]   = { n / meta.dj, n / meta.di };
            const size_t local_work_size[work_dim]    = { 8, 8 }; // FIXME: safe for Mali.
            cl_uint num_events_in_wait_list = 0;
            const cl_event *event_wait_list = NULL;

            err = clEnqueueNDRangeKernel(queue, kernel,
                work_dim, global_work_offset, global_work_size, local_work_size,
                num_events_in_wait_list, event_wait_list, &enqueue);
            assert(CL_SUCCESS == err && "clEnqueueNDRangeKernel() failed.");
        }

        // Wait for kernel completion.
        {
            const cl_uint num_events = 1;
            const cl_event event_list[num_events] = { enqueue };
            err = clWaitForEvents(num_events, event_list);
            assert(CL_SUCCESS == err && "clWaitForEvents() failed.");
        }
    } // END OF enqueue_kernel()


    // Calculate nanoseconds, flops, Gflops/s (flops/ns), etc.
    void profile_execution()
    {
        cl_int err = CL_SUCCESS;

        cl_ulong end = 0;
        err = clGetEventProfilingInfo(enqueue, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end, 0);
        assert(CL_SUCCESS == err && "clGetEventProfilingInfo() failed.");

        cl_ulong start = 0;
        err = clGetEventProfilingInfo(enqueue, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start, 0);
        assert(CL_SUCCESS == err && "clGetEventProfilingInfo() failed.");

        const cl_ulong ns = end - start;
        const cl_ulong n = (cl_ulong) args.matrix_order;
        const cl_ulong flops = 2 * (n + 1) * (n * n);
        const cl_double gflops_per_s = (cl_double) flops / (cl_double) ns;

        std::cout << "Calculating performance... " << gflops_per_s << " Gflops/s";
        std::cout << " (performed "  << flops << " flops in " << ns << " ns)." << std::endl;

#if (1 == XOPENME)
        xopenme_add_var_i(openme.var_count++, (char*) "  \"EXECUTION#ns\":%u", ns);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        xopenme_add_var_f(openme.var_count++, (char*) "  \"EXECUTION#us\":%.3f", ns * 1e-3);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        xopenme_add_var_f(openme.var_count++, (char*) "  \"EXECUTION#ms\":%.3f", ns * 1e-6);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        xopenme_add_var_f(openme.var_count++, (char*) "  \"EXECUTION#s\":%.3f", ns * 1e-9);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        xopenme_add_var_i(openme.var_count++, (char*) "  \"EXECUTION#flops\":%u", flops);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        xopenme_add_var_f(openme.var_count++, (char*) "  \"EXECUTION#Gflops\":%.3f", flops * 1e-9);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");

        xopenme_add_var_d(openme.var_count++, (char*) "  \"EXECUTION#Gflops/s\":%.3lf", gflops_per_s);
        assert(openme.var_count_below_max() && "xOpenME max var count reached.");
#endif
    } // END OF profile_execution()


    template<typename T> void _execute_kernel()
    {
        gemmbench::dataset<T> data(args.matrix_order);
        assert(data.matrix_A && "No data.");
        assert(data.matrix_B && "No data.");
        assert(data.matrix_C && "No data.");
        data.init_random();

        set_kernel_args<T>(data);
        assert(buffer_A && "No buffer.");
        assert(buffer_B && "No buffer.");
        assert(buffer_C && "No buffer.");

        enqueue_kernel();

        profile_execution();

        data.verify_results(*this);

    } // END OF _execute_kernel()

public:
    // Run the kernel on random data, profile and verify.
    void execute_kernel()
    {
        if (meta.type == "S")
        {
            _execute_kernel<cl_float>();
        }
        else if (meta.type == "D")
        {
            _execute_kernel<cl_double>();
        }
        else
        {
            assert(false && "Unsupported data type.");
        }
    } // END OF execute_kernel()

}; // END OF gemmbench::state class

} // END OF gemmbench namespace

#endif // CL_STATE_HPP
