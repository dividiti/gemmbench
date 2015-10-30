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
private:
    const static unsigned int seed = 12345;
    const static T range = static_cast<T>(1);
    const static bool zero_matrix_C = false;

    // Generate random number in [-range/2; +range/2].
    T symmetric_rand()
    {
        T zero_to_one = static_cast<T>(rand()) / static_cast<T>(RAND_MAX);
        T minus_half_to_plus_half = zero_to_one - static_cast<T>(.5);
        return minus_half_to_plus_half * range;
    }

public:
    cl_uint n;
    T * matrix_A;
    T * matrix_B;
    T * matrix_C;
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
    void init_random();

    // Compare the results against reference implementation.
    void verify_results(gemmbench::state & s, T eps = static_cast<T>(1e-5));
  
}; // END OF gemmbench::dataset class

} // END OF gemmbench namespace

#endif // CL_DATASET_HPP
