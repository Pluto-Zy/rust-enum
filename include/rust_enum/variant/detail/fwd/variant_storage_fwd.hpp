#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_FWD_VARIANT_STORAGE_FWD_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_FWD_VARIANT_STORAGE_FWD_HPP

namespace rust::detail {
template <class... AltStorages>
struct variant_storage;

template <class>
struct is_specialization_of_variant_storage : std::false_type { };

template <class... Tys>
struct is_specialization_of_variant_storage<variant_storage<Tys...>> : std::true_type { };
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_FWD_VARIANT_STORAGE_FWD_HPP
