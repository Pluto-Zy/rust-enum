#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_FWD_VARIANT_STORAGE_FWD_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_FWD_VARIANT_STORAGE_FWD_HPP

namespace rust::detail {
/// Represents the status of variant_storage.
enum class variant_storage_status {
    /// The normal case. We need to store tags and all alternatives separately.
    NORMAL,
    /// When the variant has only one state (status_count is 1), we don't need to
    /// store the tag. Note that a variant with only one alternative does not
    /// necessarily have only one state, because it may also have a valueless
    /// state.
    ///
    /// Unimplemented now.
    SINGLE_ALTERNATIVE,
    /// Using niche filling optimization to store tags into an alternative.
    NICHE_FILLING,
};

template <variant_storage_status Status, class... AltStorages>
struct variant_storage;

template <class>
struct is_specialization_of_variant_storage : std::false_type { };

template <variant_storage_status Status, class... Tys>
struct is_specialization_of_variant_storage<variant_storage<Status, Tys...>> : std::true_type { };
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_FWD_VARIANT_STORAGE_FWD_HPP
