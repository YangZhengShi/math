#ifndef STAN_MATH_PRIM_MAT_FUN_ARRAY_BUILDER_HPP
#define STAN_MATH_PRIM_MAT_FUN_ARRAY_BUILDER_HPP

#include <stan/math/prim/arr/fun/array_builder.hpp>
#include <stan/math/prim/mat/fun/promoter.hpp>
#include <stan/math/prim/mat/fun/Eigen.hpp>
#include <vector>

namespace stan {
  namespace math {

    /**
     * Structure for building up arrays of matrix elements in an expression
     * using an argumentchaining add() method and
     * a getter method array() to return the result.
     *
     * @tparam T type of matrix element
     * @tparam F type of element added
     */
    template <typename T>
    struct array_builder<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> >{
      std::vector<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> >  x_;
      array_builder() : x_() { }

      template <typename F>
      array_builder& add(const Eigen::Matrix<F, Eigen::Dynamic, Eigen::Dynamic>&
                         u) {
        Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> t;
        promoter<Eigen::Matrix<F, Eigen::Dynamic, Eigen::Dynamic>,
                 Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>
                 >::promote(u, t);
        x_.push_back(t);
        return *this;
      }

      std::vector<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> > array() {
        return x_;
      }
    };

  }
}

#endif
