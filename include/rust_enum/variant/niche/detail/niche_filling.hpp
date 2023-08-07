#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_NICHE_DETAIL_NICHE_FILLING_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_NICHE_DETAIL_NICHE_FILLING_HPP

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <type_traits>

#include <rust_enum/variant/detail/fwd/variant_fwd.hpp>
#include <rust_enum/variant/detail/status_count.hpp>
#include <rust_enum/variant/niche/niche.hpp>
#include <rust_enum/variant/niche/unsigned_max.hpp>
#include <rust_enum/variant/variant_alternative.hpp>

namespace rust::detail {
template <class... AltStorages>
class niche_filling {
    template <class Niche>
    static constexpr auto available_niche() -> std::size_t {
        return (Niche::valid_range::start - (Niche::valid_range::end + 1))
            & unsigned_max<Niche::size_in_bits>::value;
    }

    static constexpr auto find_niche_filling_variant() -> std::size_t {
        constexpr std::size_t no_niche_variant = static_cast<std::size_t>(0) - 1;

        // find the index of the largest alternative
        constexpr std::size_t storage_sizes[] = { AltStorages::storage_size... };
        // std::distance and std::max_element are constexpr in C++17
        constexpr std::size_t largest_storage_index = std::distance(
            std::begin(storage_sizes),
            std::max_element(std::begin(storage_sizes), std::end(storage_sizes))
        );
        // find the type of the largest alternative
        using largest_storage_alternative = typename variant_alternative<
            largest_storage_index,
            variant<typename AltStorages::value_type...>>::type;

        // We don't yet support using the niche of class members and variant alternatives. The
        // niche of class types will be set to empty. If we want to support class types, we should
        // find the member with the largest niche here. Note that if the member itself also has
        // multiple members, the niche of the member will be the largest niche of its members. So
        // we should also save the offset of the member.
        constexpr std::size_t available = available_niche<niche<largest_storage_alternative>>();
        constexpr std::size_t status_count =
            largest_storage_index == 0 || largest_storage_index == std::size(storage_sizes) - 1
            ? variant_status_count<AltStorages...>::value - 1
            : variant_status_count<AltStorages...>::value;

        if (status_count > available) {
            return no_niche_variant;
        }

        // Checks whether all the alternatives fit in the niche.
        for (std::size_t i = 0; i != std::size(storage_sizes); ++i) {
            if (i == largest_storage_index)
                continue;

            // Currently we don't implement the niche of class types and variant types, so the
            // offset of niche is always 0. We also don't implement the feature that put the
            // alternative after the niche, so we just return here.
            if (storage_sizes[i] > 0) {
                return no_niche_variant;
            }
        }

        return largest_storage_index;
    }

    template <class Niche>
    static constexpr auto reserve(std::size_t count) -> std::size_t {
        using valid_range = typename Niche::valid_range;
        constexpr std::size_t max_value = unsigned_max<Niche::size_in_bits>::value;
        constexpr std::size_t distance_end_zero = max_value - valid_range::end;

        if (niche_index == 0 || niche_index == sizeof...(AltStorages) - 1) {
            --count;
        }

        auto const move_start = [count] { return (valid_range::start - count) & max_value; };

        auto const move_end = [] { return (valid_range::end + 1) & max_value; };

        if (valid_range::start > valid_range::end) {
            return move_end();
        } else if (valid_range::start <= distance_end_zero) {
            if (count <= valid_range::start) {
                return move_start();
            } else {
                return move_end();
            }
        } else {
            std::size_t const end = (valid_range::end + count) & max_value;
            if (1 <= end && end <= valid_range::end) {
                return move_start();
            } else {
                return move_end();
            }
        }
    }

    struct empty {
        using type = void;
    };

    // Note that we cannot use alias template here, because the partial instantiation of alias
    // template will cause the complete instantiation of the actual type.
    template <class Niche, std::size_t StatusCount>
    struct niche_start_impl : std::integral_constant<std::size_t, reserve<Niche>(StatusCount)> { };

public:
    static constexpr std::size_t niche_index = find_niche_filling_variant();

    static constexpr bool has_niche_alternative = niche_index != static_cast<std::size_t>(0) - 1;

    using niche_type = typename std::conditional<
        has_niche_alternative,
        // We cannot use variant_alternative<index, variant<typename
        // AltStorages::value_type...>>::type here, because it will request the complete
        // instantiation of variant_alternative, which may cause overflow and compilation error.
        variant_alternative<niche_index, variant<typename AltStorages::value_type...>>,
        empty>::type::type;

    static constexpr std::size_t niche_start = std::conditional<
        has_niche_alternative,
        // We cannot use std::integral_constant<std::size_t, reserve<niche<typename
        // niche_alternative::type>>(status_count)> here, because it will request the complete
        // instantiation of reserve<>(), which may cause error.
        niche_start_impl<niche<niche_type>, variant_status_count<AltStorages...>::value>,
        std::integral_constant<std::size_t, 0>>::type::value;

    static constexpr std::size_t alt_index_to_niche_index(std::size_t index) {
        if constexpr (niche_index == 0) {
            return index - 1 + niche_start;
        } else {
            return index + niche_start;
        }
    }

    static constexpr std::size_t niche_index_to_alt_index(std::size_t index) {
        if constexpr (niche_index == 0) {
            return index - niche_start + 1;
        } else {
            return index - niche_start;
        }
    }
};
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_NICHE_DETAIL_NICHE_FILLING_HPP
