#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_USES_ALLOCATOR_SPECIALIZATION_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_USES_ALLOCATOR_SPECIALIZATION_HPP

#include <memory>
#include <type_traits>

#include <rust_enum/tuple/detail/fwd/tuple_fwd.hpp>

template <class... Tys, class Alloc>
struct std::uses_allocator<rust::tuple<Tys...>, Alloc> : std::true_type { };

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_USES_ALLOCATOR_SPECIALIZATION_HPP
