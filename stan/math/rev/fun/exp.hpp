#ifndef STAN_MATH_REV_FUN_EXP_HPP
#define STAN_MATH_REV_FUN_EXP_HPP

#include <stan/math/rev/meta.hpp>
#include <stan/math/rev/core.hpp>
#include <stan/math/rev/functor/parallel_map.hpp>
#include <stan/math/prim/fun/cos.hpp>
#include <stan/math/prim/fun/exp.hpp>
#include <stan/math/rev/fun/is_inf.hpp>
#include <stan/math/prim/fun/isinf.hpp>
#include <stan/math/prim/fun/isfinite.hpp>
#include <stan/math/rev/fun/is_nan.hpp>
#include <stan/math/rev/fun/sin.hpp>
#include <cmath>
#include <complex>

namespace stan {
namespace math {

namespace internal {
class exp_vari : public op_v_vari {
 public:
  explicit exp_vari(vari* avi) : op_v_vari(std::exp(avi->val_), avi) {}
  void chain() { avi_->adj_ += adj_ * val_; }
};
}  // namespace internal

/**
 * Return the exponentiation of the specified variable (cmath).
 *
   \f[
   \mbox{exp}(x) =
   \begin{cases}
     e^x & \mbox{if } -\infty\leq x \leq \infty \\[6pt]
     \textrm{NaN} & \mbox{if } x = \textrm{NaN}
   \end{cases}
   \f]

   \f[
   \frac{\partial\, \mbox{exp}(x)}{\partial x} =
   \begin{cases}
     e^x & \mbox{if } -\infty\leq x\leq \infty \\[6pt]
     \textrm{NaN} & \mbox{if } x = \textrm{NaN}
   \end{cases}
   \f]
 *
 * @param a Variable to exponentiate.
 * @return Exponentiated variable.
 */
inline var exp(const var& a) { return var(new internal::exp_vari(a.vi_)); }

template <typename Container,
          require_eigen_st<is_var, Container>* = nullptr>
inline auto exp(Container&& x) {
  // Declare result container
  plain_type_t<Container> result(x.rows(), x.cols());

  // Functor defining how inputs should be indexed
  auto ind_f = [&](int i, const auto& fun, const auto& x) {
    return fun(x.coeffRef(i));
  };

  // Functor defining function to be applied to indexed arguments
  auto f = [&](const auto& x) { return stan::math::exp(x); };

  return parallel_map(f, ind_f, std::forward<plain_type_t<Container>>(result),
                      std::forward_as_tuple(x));
}

/**
 * Return the exponentiation (base e) of the specified complex number.
 * @param z argument
 * @return exponentiation of argument
 */
inline std::complex<var> exp(const std::complex<var>& z) {
  return internal::complex_exp(z);
}

}  // namespace math
}  // namespace stan
#endif
