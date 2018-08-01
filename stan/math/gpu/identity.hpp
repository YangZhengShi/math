#ifndef STAN_MATH_GPU_IDENTITY_HPP
#define STAN_MATH_GPU_IDENTITY_HPP
#ifdef STAN_OPENCL
#include <stan/math/gpu/matrix_gpu.hpp>
#include <CL/cl.hpp>

namespace stan {
namespace math {

/**
 * Returns the identity matrix stored on the GPU
 *
 * @param rows_cols the number of rows and columns
 *
 * @return the identity matrix
 *
 */
inline matrix_gpu identity(int rows_cols) {
  matrix_gpu A(rows_cols, rows_cols);
  if (rows_cols == 0) {
    return A;
  }
  cl::CommandQueue cmdQueue = opencl_context.queue();

  try {
    kernel_cl kernel("identity");
    kernel.set_args(A.buffer(), A.rows(), A.cols());
    cmdQueue.enqueueNDRangeKernel(kernel.compiled_, cl::NullRange,
                                  cl::NDRange(A.rows(), A.cols()),
                                  cl::NullRange, NULL, NULL);
  } catch (const cl::Error& e) {
    check_opencl_error("identity", e);
  }
  return A;
}
}  // namespace math
}  // namespace stan

#endif
#endif
