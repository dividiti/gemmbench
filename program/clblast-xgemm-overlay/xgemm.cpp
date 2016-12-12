
// =================================================================================================
// This file is part of the CLBlast project. The project is licensed under Apache Version 2.0. This
// project loosely follows the Google C++ styleguide and uses a tab-size of two spaces and a max-
// width of 100 characters per line.
//
// Author(s):
//   Cedric Nugteren <www.cedricnugteren.nl>
//
// =================================================================================================

#include <string>
#include <iterator>
#include <fstream>

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

#include <iostream>

#include "client/client.hpp"
#include "xgemm.hpp"

// Shortcuts to the clblast namespace
using float2 = clblast::float2;
using double2 = clblast::double2;

// Main function (not within the clblast namespace)
int main(int argc, char *argv[]) {
  switch(clblast::GetPrecision(argc, argv, clblast::Precision::kSingle)) {
    case clblast::Precision::kHalf:
      clblast::RunClient<clblast::TestXgemm<half>, half, half>(argc, argv); break;
    case clblast::Precision::kSingle:
      clblast::RunClient<clblast::TestXgemm<float>, float, float>(argc, argv); break;
    case clblast::Precision::kDouble:
      clblast::RunClient<clblast::TestXgemm<double>, double, double>(argc, argv); break;
    case clblast::Precision::kComplexSingle:
      clblast::RunClient<clblast::TestXgemm<float2>, float2, float2>(argc, argv); break;
    case clblast::Precision::kComplexDouble:
      clblast::RunClient<clblast::TestXgemm<double2>, double2, double2>(argc, argv); break;
  }
  return 0;
}

// trim from start
static inline std::string ltrim(std::string s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string rtrim(std::string s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string trim(std::string s) {
    return ltrim(rtrim(s));
}

namespace clblast {

namespace overlay {

// Constructor: forwards to base class constructor
template <typename T>
Xgemm<T>::Xgemm(Queue &queue, const std::string& kernelSource, EventPointer event, const std::string &name):
    Routine(queue, event, name, PrecisionValue<T>(), {kernelSource.c_str()}) {
}

// =================================================================================================

// The main routine
template <typename T>
void Xgemm<T>::DoGemm(const std::vector<size_t>& global,
                      const std::vector<size_t>& local,
                      const std::string& kernelName,
                      std::string argumentOrder,
                      const Layout layout,
                      const Transpose a_transpose, const Transpose b_transpose,
                      const size_t m, const size_t n, const size_t k,
                      const T alpha,
                      const Buffer<T> &a_buffer, const size_t a_offset, const size_t a_ld,
                      const Buffer<T> &b_buffer, const size_t b_offset, const size_t b_ld,
                      const T beta,
                      const Buffer<T> &c_buffer, const size_t c_offset, const size_t c_ld) {

  // Makes sure all dimensions are larger than zero
  if ((m == 0) || (n == 0) || (k == 0)) { throw BLASError(StatusCode::kInvalidDimension); }

  // Computes whether or not the matrices are transposed in memory. This is based on their layout
  // (row or column-major) and whether or not they are requested to be pre-transposed. Note
  // that the Xgemm kernel expects either matrices A and C (in case of row-major) or B (in case of
  // col-major) to be transformed, so transposing requirements are not the same as whether or not
  // the matrix is actually transposed in memory.
  const auto a_rotated = (layout == Layout::kColMajor && a_transpose != Transpose::kNo) ||
                         (layout == Layout::kRowMajor && a_transpose == Transpose::kNo);
  const auto b_rotated = (layout == Layout::kColMajor && b_transpose != Transpose::kNo) ||
                         (layout == Layout::kRowMajor && b_transpose == Transpose::kNo);
  const auto c_rotated = (layout == Layout::kRowMajor);
  static const auto a_want_rotated = false;
  static const auto b_want_rotated = true;
  static const auto c_want_rotated = false;
  const auto a_do_transpose = a_rotated != a_want_rotated;
  const auto b_do_transpose = b_rotated != b_want_rotated;
  const auto c_do_transpose = c_rotated != c_want_rotated;

  // In case of complex data-types, the transpose can also become a conjugate transpose
  const auto a_conjugate = (a_transpose == Transpose::kConjugate);
  const auto b_conjugate = (b_transpose == Transpose::kConjugate);

  // Retrieves the proper XgemmDirect kernel from the compiled binary
  //const auto name = (a_do_transpose) ? (b_do_transpose ? "XgemmDirectTT" : "XgemmDirectTN") :
                                       //(b_do_transpose ? "XgemmDirectNT" : "XgemmDirectNN");
  std::cout << "creating kernel\n";
  auto kernel = Kernel(*program_, kernelName);
  std::cout << "finished creating kernel\n";

  size_t i = 0;
  auto setArg = [&](std::string arg) {
    arg = trim(arg);
    if (arg == "m") { kernel.SetArgument(i, static_cast<int>(m)); } else
    if (arg == "n") { kernel.SetArgument(i, static_cast<int>(n)); } else
    if (arg == "k") { kernel.SetArgument(i, static_cast<int>(k)); } else
    if (arg == "a") { kernel.SetArgument(i, a_buffer()); } else
    if (arg == "b") { kernel.SetArgument(i, b_buffer()); } else
    if (arg == "c") { kernel.SetArgument(i, c_buffer()); } 
    i = i+1;
  };

  // parse and sets the kernel arguments
  std::string delimiter(",");
  size_t pos = 0;
  while ((pos = argumentOrder.find(delimiter)) != std::string::npos) {
    setArg(argumentOrder.substr(0, pos));
    argumentOrder.erase(0, pos + delimiter.length());
  }
  setArg(argumentOrder); // set last arg

  std::cout << "finished setting " << i << " arguments\n";

  // Launches the kernel
  RunKernel(kernel, queue_, device_, global, local, event_);
}

// Compiles the templated class
template class Xgemm<half>;
template class Xgemm<float>;
template class Xgemm<double>;
template class Xgemm<float2>;
template class Xgemm<double2>;

}

}

// =================================================================================================
