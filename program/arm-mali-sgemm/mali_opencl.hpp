/*
 * Copyright (c) 2016 ARM Limited.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef __MALI_OPENCL_H__
#define __MALI_OPENCL_H__

/* Configure the Khronos C++ wrapper to target OpenCL 1.1: */
#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_TARGET_OPENCL_VERSION 110
#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#include <CL/cl2.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <cstdlib>

namespace mali
{
typedef std::vector<uint8_t> ImageData;

#define ERROR( msg )\
    do\
    {\
        std::stringstream __str;\
        __str << " ERROR in "<<__FILE__<<":"<<__LINE__<<" : "<<msg;\
        errno=0; /*clear error code from previous calls */ \
        throw std::runtime_error( __str.str() );\
    }while(0)

#define ERROR_ON_MSG( cond, msg )\
    do\
    {\
        if( cond )\
        {\
            ERROR( msg );\
        }\
    }while(0)

#define ERROR_ON( cond )\
    ERROR_ON_MSG( cond, #cond )


#define READ_CONFIG( attr, env, default_value )\
    do{\
        char *str = getenv( env );\
        attr = ( str != NULL ) ? atoi( str ) : default_value;\
        printf("%s = %d\n",env,(int) attr);\
    }while(0)

static int round_up(int a, int b);

static std::string read_file(const std::string &filename, bool binary = false);

static void save_to_ppm(const unsigned char *in, int width, int height, const std::string &filename, bool input_is_grayscale=false);

static int string_to_int(std::string::iterator &it);

static void discard_comments( std::string::iterator &it);

static void discard_comments_and_spaces( std::string::iterator &it);

static void read_from_ppm(const std::string &filename, int &width, int &height, ImageData &img, bool convert_to_grayscale=false);

static void build_cl_program( cl::Program &program, const char *options = NULL);

typedef void (mali_sample)(int argc,  const char **argv);

static int run_mali_sample( int argc, const char **argv, mali_sample &func, const char *sample_name);


/************* Implementation ****************/

static int round_up(int a, int b)
{
    int rem = a % b;

    if(rem == 0)
        return a;

    a -= rem;
    a += b;
    return a;
}

static std::string read_file(const std::string &filename, bool binary)
{
    std::string out;
    std::ifstream fs;
    try
    {
        fs.exceptions( std::ifstream::failbit | std::ifstream::badbit );
        std::ios_base::openmode mode = std::ios::in;
        if( binary )
            mode |= std::ios::binary;
        fs.open( filename, mode );

        fs.seekg(0, std::ios::end);
        out.reserve(fs.tellg());
        fs.seekg(0, std::ios::beg);

        out.assign( std::istreambuf_iterator<char>(fs), std::istreambuf_iterator<char>());
    }
    catch( std::ifstream::failure e)
    {
        throw std::runtime_error(std::string("accessing ") + filename);
    }

    return out;
}

static void save_to_ppm(const unsigned char *in, int width, int height, const std::string &filename, bool input_is_grayscale)
{
    std::ofstream fs;
    try
    {
        fs.exceptions( std::ofstream::failbit | std::ofstream::badbit | std::ofstream::eofbit );
        fs.open( filename, std::ios::out | std::ios::binary);

        fs << "P6" << std::endl << width << " " << height << " 255" << std::endl;
        if( input_is_grayscale )
        {
            for( int i=0; i < width * height; i++)
            {
                fs.put(in[i]);
                fs.put(in[i]);
                fs.put(in[i]);
            }
        }
        else
        {
            fs.write( (const char *) in, width * height * 3);
        }
        fs.close();
    }
    catch( std::ofstream::failure e)
    {
        throw std::runtime_error(std::string("writing ") + filename);
    }
}

static int string_to_int(std::string::iterator &it)
{
    std::string val;
    for(; !isspace(*it) || *it == '#'; it++)
    {
        val = val + *it;
    }

    return atoi(val.c_str());
}

static void discard_comments( std::string::iterator &it)
{
    for(; *it == '#' ; it++) // While line starts with '#'
    {
        for( ; *it != '\n'; it++ ); // Discard the rest of the line
    }
}

static void discard_comments_and_spaces( std::string::iterator &it)
{
    while(1)
    {
        discard_comments(it);
        if( !isspace(*it) )
            break;
        it++;
    }
}

static void read_from_ppm(const std::string &filename, int &width, int &height, ImageData &img, bool convert_to_grayscale)
{
    char magic_number[2];

    std::string data  = read_file(filename, true);
    std::string::iterator it = data.begin();
    magic_number[0] = *it; it++;
    magic_number[1] = *it; it++;

    if(magic_number[0] != 'P' || magic_number[1] != '6')
        throw std::runtime_error(std::string("invalid file type with file ") + filename);

    //discard whitespace
    discard_comments_and_spaces(it);

    width = string_to_int(it);

    discard_comments_and_spaces(it);

    height = string_to_int(it);

    discard_comments_and_spaces(it);

    int max_val = string_to_int(it);

    if(max_val >= 256)
        throw std::runtime_error(std::string("2 bytes per colour channel not supported in file ") + filename);

    discard_comments(it);

    //c is now next character, must be whitespace
    if(!isspace(*it))
        throw std::runtime_error(std::string("invalid ppm header in file ") + filename);
    it++;

    if( data.end() - it < (int)(width*height*3))
        throw std::runtime_error(std::string("Not enough data in file ") + filename);

    const unsigned char* file_data = (const unsigned char*)(data.c_str() + (it - data.begin()));
    if( convert_to_grayscale )
    {
        img.resize( width * height );
        for( int y=0; y < height; y++)
        {
            for( int x=0; x < width; x++)
            {
                const unsigned char* pixel = &file_data[(y*width+x)*3];
                img[y*width+x] = 0.2126f * pixel[0] +  0.7152f * pixel[1] +  0.0722f  * pixel[2];
            }
        }
    }
    else
    {
        img.resize( width * height * 3);
        memcpy( img.data(),  file_data, img.size());
    }
}

void build_cl_program( cl::Program &program, const char *options)
{
    try
    {
        program.build(options);
    }
    catch( cl::Error err)
    {
        std::stringstream ss;
        std::string str;
        ss << err.what() << "(" << err.err() << ")" << std::endl;
        ss << "Build Options:\t" << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(cl::Device::getDefault()) << std::endl;
        ss << "Build Log:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(cl::Device::getDefault()) << std::endl;
        str = ss.str();
        errno = 0;
        throw std::runtime_error(str.c_str());
    }
}

static int run_mali_sample( int argc, const char **argv, mali_sample &func, const char *test_name)
{
    printf("\n%s\n\n", test_name);
    try
    {
        func(argc, argv);

        printf("\nTest passed\n");
        return 0;
    }
    catch (cl::Error err)
    {
        std::cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        std::cerr << std::endl << "ERROR " << err.what() << "(" << err.err() << ")" << std::endl;
        std::cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    }
    catch(std::runtime_error &err )
    {
        std::cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        std::cerr << std::endl << "ERROR " << err.what() << " " << (errno ? strerror(errno) : "") << std::endl;
        std::cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    }

    printf("\nTest FAILED\n");
    return -1;
}

}
#endif /* __MALI_OPENCL_H__ */
