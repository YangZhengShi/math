#ifndef STAN_MATH_REV_META_VAR_TO_VARI_FILTER_HPP
#define STAN_MATH_REV_META_VAR_TO_VARI_FILTER_HPP

#include <stan/math/rev/meta/is_var.hpp>
namespace stan {
// NOTE: This should probably be in Stan namespace not math
namespace math {

namespace internal {
struct tuple_cat_caller {
  template <typename... Ts>
  auto operator()(Ts&&... args) {
    return std::tuple_cat(std::forward<Ts>(args)...);
  }
};

template <template <typename...> class Pred,
          template <typename...> class Filter, typename T>
using var_filter_helper
    = std::conditional_t<Pred<std::decay_t<T>>::value, std::tuple<Filter<T>>,
                         std::tuple<std::nullptr_t>>;

template <typename T>
using container_var_vari_value_t
    = std::conditional_t<is_container<std::decay_t<T>>::value, vari**, vari*>;

template <typename T>
using contains_var_value = is_var<scalar_type_t<T>>;
}  // namespace internal

template <typename... Ts>
using var_to_vari_filter_t = std::result_of_t<internal::tuple_cat_caller(
    internal::var_filter_helper<internal::contains_var_value,
                                internal::container_var_vari_value_t, Ts>...)>;

}  // namespace math
}  // namespace stan
#endif