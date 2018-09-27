#ifndef STAN_MATH_GPU_KERNELS_LOWER_TRI_INVERSE_HPP
#define STAN_MATH_GPU_KERNELS_LOWER_TRI_INVERSE_HPP
#ifdef STAN_OPENCL

#include <stan/math/gpu/kernel_cl.hpp>

namespace stan {
namespace math {
namespace opencl_kernels {
// \cond
const char* lower_tri_inverse_step2_kernel_code = STRINGIFY(
    // \endcond
    /**
     * Calculates the intermediate products in the lower_tri_inverse
     *
     * @param[in] A input matrix that is being inverted.
     * @param[out] C output matrix with results of the batched matrix
     * multiplications
     * @param M The number of rows for A.
     * @param rows The number of rows in a single matrix of the batch
     * @param non_padded_rows The number of rows in the non-padded matrix
     * @note Code is a <code>const char*</code> held in
     * <code>lower_tri_inverse_step2_kernel_code.</code>
     *  Used in math/gpu/lower_tri_inverse.hpp.
     *  This kernel uses the helper macros available in helpers.cl.
     */
    __kernel void lower_tri_inverse_step2(__global read_only double* A,
                                          __global double* C, const int M,
                                          const int rows, int non_padded_rows) {
      int t = get_global_id(2);
      int offset = t * rows * 2;
      // thread index inside the thread_block
      const int thread_block_row = get_local_id(0);
      const int thread_block_col = get_local_id(1);
      // global thread index
      const int global_thread_row = THREAD_BLOCK_SIZE * get_group_id(0) + thread_block_row;
      const int global_thread_col = THREAD_BLOCK_SIZE * get_group_id(1) + thread_block_col;

      // local memory
      __local double A_local[THREAD_BLOCK_SIZE][THREAD_BLOCK_SIZE];
      __local double B_local[THREAD_BLOCK_SIZE][THREAD_BLOCK_SIZE];

      double acc[WORK_PER_THREAD];
      for (int w = 0; w < WORK_PER_THREAD; w++) {
        acc[w] = 0.0;
      }

      const int num_tiles
          = (rows + THREAD_BLOCK_SIZE - 1) / THREAD_BLOCK_SIZE;
      // iterate over all tiles
      for (int tile_ind = 0; tile_ind < num_tiles; tile_ind++) {
        // each thread copies WORK_PER_THREAD values to the local
        // memory
        for (int w = 0; w < WORK_PER_THREAD; w++) {
          const int tiled_i = THREAD_BLOCK_SIZE * tile_ind + thread_block_row;
          const int tiled_j = THREAD_BLOCK_SIZE * tile_ind + thread_block_col;
          if ((offset + rows + tiled_j + w * THREAD_BLOCK_SIZE_COL)
                  <= (offset + global_thread_row + rows)
              && (offset + rows + tiled_j + w * THREAD_BLOCK_SIZE_COL)
                     < non_padded_rows
              && (offset + global_thread_row + rows) < non_padded_rows) {
            A_local[thread_block_col + w * THREAD_BLOCK_SIZE_COL]
                   [thread_block_row]
                = A[(offset + rows + tiled_j + w * THREAD_BLOCK_SIZE_COL) * M
                    + offset + global_thread_row + rows];
          } else {
            A_local[thread_block_col + w * THREAD_BLOCK_SIZE_COL]
                   [thread_block_row]
                = 0;
          }
          if (((tiled_i + rows + offset) < non_padded_rows)
              && ((offset + global_thread_col + w * THREAD_BLOCK_SIZE_COL)
                  <= (tiled_i + rows + offset))
              && (offset + global_thread_col + w * THREAD_BLOCK_SIZE_COL) < non_padded_rows
              && (tiled_i + rows + offset) < non_padded_rows) {
            B_local[thread_block_col + w * THREAD_BLOCK_SIZE_COL]
                   [thread_block_row]
                = A[(offset + global_thread_col + w * THREAD_BLOCK_SIZE_COL) * M + tiled_i
                    + rows + offset];
          } else {
            B_local[thread_block_col + w * THREAD_BLOCK_SIZE_COL]
                   [thread_block_row]
                = 0.0;
          }
        }
        // wait until all tile values are loaded to the local memory
        barrier(CLK_LOCAL_MEM_FENCE);
        for (int block_ind = 0; block_ind < THREAD_BLOCK_SIZE; block_ind++) {
          for (int w = 0; w < WORK_PER_THREAD; w++) {
            acc[w] += A_local[block_ind][thread_block_row]
                      * B_local[thread_block_col + w * THREAD_BLOCK_SIZE_COL]
                               [block_ind];
          }
        }
        barrier(CLK_LOCAL_MEM_FENCE);
      }
      // save the values
      for (int w = 0; w < WORK_PER_THREAD; w++) {
        // each thread saves WORK_PER_THREAD values
        C[t * rows * rows + (global_thread_col + w * THREAD_BLOCK_SIZE_COL) * rows + global_thread_row]
            = acc[w];
      }
    }
    // \cond
);
// \endcond

/**
 * See the docs for \link kernels/matrix_multiply.hpp add() \endlink
 */
const local_range_kernel<cl::Buffer, cl::Buffer, int, int, int>
    lower_tri_inverse_step2("lower_tri_inverse_step2",
                            lower_tri_inverse_step2_kernel_code,
                            {{"THREAD_BLOCK_SIZE", 32},
                             {"WORK_PER_THREAD", 8}});

}  // namespace opencl_kernels
}  // namespace math
}  // namespace stan
#endif
#endif
