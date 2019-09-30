#ifndef STAN_MATH_PRIM_ARR_FUN_LOG_SUM_EXP_HPP
#define STAN_MATH_PRIM_ARR_FUN_LOG_SUM_EXP_HPP

#include <stan/math/prim/meta.hpp>
#include <algorithm>
#include <numeric>
#include <limits>
#include <vector>
#include <cmath>
#include <cstdlib>

namespace stan {
namespace math {

/**
 * Return the log of the sum of the exponentiated values of the specified
 * sequence of values.
 *
 * The function is defined as follows to prevent overflow in exponential
 * calculations.
 *
 * \f$\log \sum_{n=1}^N \exp(x_n) = \max(x) + \log \sum_{n=1}^N \exp(x_n -
 * \max(x))\f$.
 *
 * @param[in] x array of specified values
 * @return The log of the sum of the exponentiated vector values.
 */
inline double log_sum_exp(const std::vector<double>& x) {
  using std::exp;
  using std::log;
  using std::numeric_limits;
  double max_val = *std::max_element(x.begin(), x.end());
  double sum = std::accumulate(
      x.begin(), x.end(), 0.0,
      [&max_val](auto& acc, auto&& x_i) { return acc + exp(x_i - max_val); });
  return max_val + log(sum);
}

}  // namespace math
}  // namespace stan

#endif
