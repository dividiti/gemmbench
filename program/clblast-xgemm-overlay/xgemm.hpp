
// =================================================================================================
// This file is part of the CLBlast project. The project is licensed under Apache Version 2.0. This
// project loosely follows the Google C++ styleguide and uses a tab-size of two spaces and a max-
// width of 100 characters per line.
//
// Author(s):
//   Cedric Nugteren <www.cedricnugteren.nl>
//
// This file implements a class with static methods to describe the Xgemm routine. Examples of
// such 'descriptions' are how to calculate the size a of buffer or how to run the routine. These
// static methods are used by the correctness tester and the performance tester.
//
// =================================================================================================

#ifndef CLBLAST_TEST_ROUTINES_XGEMM_H_
#define CLBLAST_TEST_ROUTINES_XGEMM_H_

#include <vector>
#include <string>

#ifdef CLBLAST_REF_CLBLAS
  #include "test/wrapper_clblas.hpp"
#endif
#ifdef CLBLAST_REF_CBLAS
  #include "test/wrapper_cblas.hpp"
#endif

namespace clblast {
// =================================================================================================

// See comment at top of file for a description of the class
template <typename T>
class TestXgemm {
 public:

  // The BLAS level: 1, 2, or 3
  static size_t BLASLevel() { return 3; }

  // The list of arguments relevant for this routine
  static std::vector<std::string> GetOptions() {
    return {kArgM, kArgN, kArgK,
            kArgLayout, kArgATransp, kArgBTransp,
            kArgALeadDim, kArgBLeadDim, kArgCLeadDim,
            kArgAOffset, kArgBOffset, kArgCOffset,
            kArgAlpha, kArgBeta};
  }

  // Describes how to obtain the sizes of the buffers
  static size_t GetSizeA(const Arguments<T> &args) {
    auto a_rotated = (args.layout == Layout::kColMajor && args.a_transpose != Transpose::kNo) ||
                     (args.layout == Layout::kRowMajor && args.a_transpose == Transpose::kNo);
    auto a_two = (a_rotated) ? args.m : args.k;
    return a_two * args.a_ld + args.a_offset;
  }
  static size_t GetSizeB(const Arguments<T> &args) {
    auto b_rotated = (args.layout == Layout::kColMajor && args.b_transpose != Transpose::kNo) ||
                     (args.layout == Layout::kRowMajor && args.b_transpose == Transpose::kNo);
    auto b_two = (b_rotated) ? args.k : args.n;
    return b_two * args.b_ld + args.b_offset;
  }
  static size_t GetSizeC(const Arguments<T> &args) {
    auto c_rotated = (args.layout == Layout::kRowMajor);
    auto c_two = (c_rotated) ? args.m : args.n;
    return c_two * args.c_ld + args.c_offset;
  }

  // Describes how to set the sizes of all the buffers
  static void SetSizes(Arguments<T> &args) {
    args.a_size = GetSizeA(args);
    args.b_size = GetSizeB(args);
    args.c_size = GetSizeC(args);
  }

  // Describes what the default values of the leading dimensions of the matrices are
  static size_t DefaultLDA(const Arguments<T> &args) { return args.k; }
  static size_t DefaultLDB(const Arguments<T> &args) { return args.n; }
  static size_t DefaultLDC(const Arguments<T> &args) { return args.n; }

  // Describes which transpose options are relevant for this routine
  using Transposes = std::vector<Transpose>;
  static Transposes GetATransposes(const Transposes &all) { return all; }
  static Transposes GetBTransposes(const Transposes &all) { return all; }

  // Describes how to run the CLBlast routine
  static StatusCode RunRoutine(const Arguments<T> &args, Buffers<T> &buffers, Queue &queue) {
    auto queue_plain = queue();
    auto event = cl_event{};
    auto status = Gemm(args.layout, args.a_transpose, args.b_transpose,
                       args.m, args.n, args.k, args.alpha,
                       buffers.a_mat(), args.a_offset, args.a_ld,
                       buffers.b_mat(), args.b_offset, args.b_ld, args.beta,
                       buffers.c_mat(), args.c_offset, args.c_ld,
                       &queue_plain, &event);
    clWaitForEvents(1, &event);
    return status;
  }

  // Describes how to run the clBLAS routine (for correctness/performance comparison)
  #ifdef CLBLAST_REF_CLBLAS
    static StatusCode RunReference1(const Arguments<T> &args, Buffers<T> &buffers, Queue &queue) {
      auto queue_plain = queue();
      auto event = cl_event{};
      auto status = clblasXgemm(convertToCLBLAS(args.layout),
                                convertToCLBLAS(args.a_transpose),
                                convertToCLBLAS(args.b_transpose),
                                args.m, args.n, args.k, args.alpha,
                                buffers.a_mat, args.a_offset, args.a_ld,
                                buffers.b_mat, args.b_offset, args.b_ld, args.beta,
                                buffers.c_mat, args.c_offset, args.c_ld,
                                1, &queue_plain, 0, nullptr, &event);
      clWaitForEvents(1, &event);
      return static_cast<StatusCode>(status);
    }
  #endif

  // Describes how to run the CPU BLAS routine (for correctness/performance comparison)
  #ifdef CLBLAST_REF_CBLAS
    static StatusCode RunReference2(const Arguments<T> &args, Buffers<T> &buffers, Queue &queue) {
      std::vector<T> a_mat_cpu(args.a_size, static_cast<T>(0));
      std::vector<T> b_mat_cpu(args.b_size, static_cast<T>(0));
      std::vector<T> c_mat_cpu(args.c_size, static_cast<T>(0));
      buffers.a_mat.Read(queue, args.a_size, a_mat_cpu);
      buffers.b_mat.Read(queue, args.b_size, b_mat_cpu);
      buffers.c_mat.Read(queue, args.c_size, c_mat_cpu);
      cblasXgemm(convertToCBLAS(args.layout),
                 convertToCBLAS(args.a_transpose),
                 convertToCBLAS(args.b_transpose),
                 args.m, args.n, args.k, args.alpha,
                 a_mat_cpu, args.a_offset, args.a_ld,
                 b_mat_cpu, args.b_offset, args.b_ld, args.beta,
                 c_mat_cpu, args.c_offset, args.c_ld);
      buffers.c_mat.Write(queue, args.c_size, c_mat_cpu);
      return StatusCode::kSuccess;
    }
  #endif

  // Describes how to download the results of the computation (more importantly: which buffer)
  static std::vector<T> DownloadResult(const Arguments<T> &args, Buffers<T> &buffers, Queue &queue) {
    std::vector<T> result(args.c_size, static_cast<T>(0));
    buffers.c_mat.Read(queue, args.c_size, result);
    return result;
  }

  // Describes how to compute the indices of the result buffer
  static size_t ResultID1(const Arguments<T> &args) { return args.m; }
  static size_t ResultID2(const Arguments<T> &args) { return args.n; }
  static size_t GetResultIndex(const Arguments<T> &args, const size_t id1, const size_t id2) {
    return (args.layout == Layout::kRowMajor) ?
           id1*args.c_ld + id2 + args.c_offset:
           id2*args.c_ld + id1 + args.c_offset;
  }

  // Describes how to compute performance metrics
  static size_t GetFlops(const Arguments<T> &args) {
    return 2 * args.m * args.n * args.k;
  }
  static size_t GetBytes(const Arguments<T> &args) {
    return (args.m*args.k + args.k*args.n + 2*args.m*args.n) * sizeof(T);
  }
};

// =================================================================================================
} // namespace clblast

// CLBLAST_TEST_ROUTINES_XGEMM_H_
#endif
