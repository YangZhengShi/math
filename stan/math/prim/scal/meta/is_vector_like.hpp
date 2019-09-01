#ifndef STAN_MATH_PRIM_SCAL_META_IS_VECTOR_LIKE_HPP
#define STAN_MATH_PRIM_SCAL_META_IS_VECTOR_LIKE_HPP

#include <stan/math/prim/scal/meta/bool_constant.hpp>
#include <stan/math/prim/scal/meta/is_eigen.hpp>
#include <stan/math/prim/scal/meta/is_vector.hpp>
#include <type_traits>

namespace stan {

/**
 * Template metaprogram indicates whether a type is vector_like.
 *
 * A type is vector_like if an instance can be accessed like a
 * vector, i.e. square brackets.
 *
 * Access is_vector_like::value for the result.
 *
 * Default behavior is to use the is_vector template metaprogram.
 *
 * @tparam T Type to test
 */
template <typename T>
struct is_vector_like
    : bool_constant<stan::is_vector_v<T> || std::is_pointer<T>::value
                    || is_eigen_v<T>> {};

template<class T>
constexpr bool is_vector_like_v = is_vector_like<T>::value;

}  // namespace stan
#endif
