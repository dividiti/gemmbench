// (c) 2015, dividiti
// BSD license

#ifndef CL_DATASET_HPP
#define CL_DATASET_HPP

#include <CL/cl.h>

#include <cstdlib>

namespace gemmbench
{

class state;

template <class T>
class dataset
{
public:
    // Order of square (i.e. M[n][n]) matrix.
    cl_uint n;

    // Matrices.
    T * matrix_A;
    T * matrix_B;
    T * matrix_C;

    // Scalars.
    T alpha;
    T beta;

    // Constructor.
    dataset(cl_uint matrix_order) :
        n(matrix_order),
        matrix_A(NULL),
        matrix_B(NULL),
        matrix_C(NULL),
        alpha(static_cast<T>(0)),
        beta(static_cast<T>(0))
    {
        matrix_A = new T[n * n];
        matrix_B = new T[n * n];
        matrix_C = new T[n * n];
    }

    // Destructor.
    ~dataset()
    {
        delete [] matrix_A;
        delete [] matrix_B;
        delete [] matrix_C;
    }

    // Initialize the data in a random way.
    void init_random(unsigned int seed = 12345, bool zero_matrix_C = false);

    // Compare the results against reference implementation.
    void verify_results(gemmbench::state & s);

private:
    // Generate random number in [-range/2; +range/2].
    T symmetric_rand(T range = static_cast<T>(1))
    {
        T zero_to_one = static_cast<T>(rand()) / static_cast<T>(RAND_MAX);
        T minus_half_to_plus_half = zero_to_one - static_cast<T>(.5);
        return minus_half_to_plus_half * range;
    }

    // Compute linear index for transposed and non-transposed matrices.
    cl_uint index(cl_uint i, cl_uint j, bool transposed)
    {
        if (!transposed)
        {
            return i*n + j;
        }
        else
        {
            return j*n + i;
        }
    }

}; // END OF gemmbench::dataset class

} // END OF gemmbench namespace

#endif // CL_DATASET_HPP
