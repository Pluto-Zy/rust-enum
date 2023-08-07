#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_VARIANT_STORAGE_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_VARIANT_STORAGE_HPP

#include <cassert>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <utility>

#include <rust_enum/variant/detail/destructible_uninitialized_union.hpp>
#include <rust_enum/variant/detail/fwd/tagged_visit_fwd.hpp>
#include <rust_enum/variant/detail/fwd/variant_storage_fwd.hpp>
#include <rust_enum/variant/detail/macro.hpp>
#include <rust_enum/variant/detail/status_count.hpp>
#include <rust_enum/variant/detail/valueless.hpp>
#include <rust_enum/variant/niche/detail/niche_filling.hpp>
#include <rust_enum/variant/valueless_strategy.hpp>

namespace rust::detail {
template <std::size_t StatusCount>
struct variant_index :
    std::conditional<
        StatusCount <= (std::numeric_limits<std::uint8_t>::max)(),
        std::uint8_t,
        typename std::conditional<
            StatusCount <= (std::numeric_limits<std::uint16_t>::max)(),
            std::uint16_t,
            std::uint32_t>::type> { };

template <variant_storage_status Status, class... AltStorages>
struct variant_storage_internal;

template <class... AltStorages>
struct variant_storage_internal<variant_storage_status::NORMAL, AltStorages...> {
    using internal_storage_t = variant_destructible_uninitialized_union_t<AltStorages...>;

    static constexpr bool no_valueless_state =
        variant_no_valueless_state<variant<typename AltStorages::value_type...>>::value;
    // If the current variant will not be valueless, no storage is required for it.
    static constexpr std::size_t status_count = variant_status_count<AltStorages...>::value;
    using variant_index_t = typename variant_index<status_count>::type;
    // cannot use variant_index_t, which may cause overflow
    static constexpr std::size_t valueless_index = status_count - 1;

    internal_storage_t _value_storage;
    variant_index_t _index;

    CONSTEXPR17 variant_storage_internal() noexcept : _value_storage(), _index(0) { }

    template <std::size_t Idx, class... Args>
    CONSTEXPR17 explicit variant_storage_internal(
        std::in_place_index_t<Idx> _tag,
        Args&&... args
    ) noexcept(  //
        std::is_nothrow_constructible<  //
            internal_storage_t,
            std::in_place_index_t<Idx>,
            Args&&...>::value
    ) :
        _value_storage(_tag, std::forward<Args>(args)...), _index(Idx) { }

    template <std::size_t Idx>
    CONSTEXPR20 void destroy_union_alt() noexcept {
        _value_storage.template destroy_content<Idx>();
    }

    [[nodiscard]] CONSTEXPR17 auto index() const noexcept -> std::size_t {
        return static_cast<std::size_t>(_index);
    }

    template <std::size_t Idx>
    CONSTEXPR17 void set_index() noexcept {
        _index = Idx;
    }

    CONSTEXPR17 auto value_storage() & noexcept -> internal_storage_t& {
        return _value_storage;
    }
    CONSTEXPR17 auto value_storage() const& noexcept -> const internal_storage_t& {
        return _value_storage;
    }
    CONSTEXPR17 auto value_storage() && noexcept -> internal_storage_t&& {
        return std::move(_value_storage);
    }
    CONSTEXPR17 auto value_storage() const&& noexcept -> const internal_storage_t&& {
        return std::move(_value_storage);
    }
};

template <class... AltStorages>
struct variant_storage_internal<variant_storage_status::NICHE_FILLING, AltStorages...> {
    using internal_storage_t = variant_destructible_uninitialized_union_t<AltStorages...>;

    static constexpr bool no_valueless_state =
        variant_no_valueless_state<variant<typename AltStorages::value_type...>>::value;
    static constexpr std::size_t status_count = variant_status_count<AltStorages...>::value;
    static constexpr std::size_t valueless_index = status_count - 1;

    using niche_impl = niche_filling<AltStorages...>;
    static_assert(
        niche_impl::has_niche_alternative,
        "No niche alternative found for the variant."
    );

    internal_storage_t _value_storage;

    variant_storage_internal() = default;

    template <std::size_t Idx, class... Args>
    CONSTEXPR17 explicit variant_storage_internal(
        std::in_place_index_t<Idx> _tag,
        Args&&... args
    ) noexcept(  //
        std::is_nothrow_constructible<  //
            internal_storage_t,
            std::in_place_index_t<Idx>,
            Args&&...>::value
    ) :
        _value_storage(_tag, std::forward<Args>(args)...) {
        set_index<Idx>();
    }

    template <std::size_t Idx>
    CONSTEXPR20 void destroy_union_alt() noexcept {
        _value_storage.template destroy_content<Idx>();
    }

    [[nodiscard]] CONSTEXPR17 std::size_t index() const noexcept {
        using niche_alt_type = typename niche_impl::niche_type;
        constexpr std::size_t niche_alt_index = niche_impl::niche_index;

        auto&& value = get_variant_content<niche_alt_index>(_value_storage).raw_value();
        if (niche<niche_alt_type>::is_niche_value(std::forward<decltype(value)>(value))) {
            std::size_t const alt_index = niche_impl::niche_index_to_alt_index(
                niche<niche_alt_type>::get_niche(std::forward<decltype(value)>(value))
            );

            assert(
                alt_index < status_count && "Invalid alternative index returned by get_niche()"
            );

            return alt_index;
        } else {
            return niche_impl::niche_index;
        }
    }

    template <std::size_t Idx>
    CONSTEXPR17 void set_index() noexcept {
        if constexpr (Idx != niche_impl::niche_index) {
            niche<typename niche_impl::niche_type>::set_niche(
                get_variant_content<niche_impl::niche_index>(_value_storage).value,
                niche_impl::alt_index_to_niche_index(Idx)
            );
        }
    }

    CONSTEXPR17 auto value_storage() & noexcept -> internal_storage_t& {
        return _value_storage;
    }
    CONSTEXPR17 auto value_storage() const& noexcept -> const internal_storage_t& {
        return _value_storage;
    }
    CONSTEXPR17 auto value_storage() && noexcept -> internal_storage_t&& {
        return std::move(_value_storage);
    }
    CONSTEXPR17 auto value_storage() const&& noexcept -> const internal_storage_t&& {
        return std::move(_value_storage);
    }
};

template <variant_storage_status Status, class... AltStorages>
struct variant_storage : variant_storage_internal<Status, AltStorages...> {
    using base = variant_storage_internal<Status, AltStorages...>;

    using base::base;

    CONSTEXPR17 auto storage() & noexcept -> variant_storage& {
        return *this;
    }
    CONSTEXPR17 auto storage() const& noexcept -> const variant_storage& {
        return *this;
    }
    CONSTEXPR17 auto storage() && noexcept -> variant_storage&& {
        return std::move(*this);
    }
    CONSTEXPR17 auto storage() const&& noexcept -> const variant_storage&& {
        return std::move(*this);
    }

    [[nodiscard]] CONSTEXPR17 auto valueless_by_exception() const noexcept -> bool {
        if constexpr (base::no_valueless_state)
            return false;
        else
            return base::index() == base::valueless_index;
    }

    template <std::size_t Idx, class... Args>
    CONSTEXPR20 void construct_union_alt(Args&&... args) noexcept(  //
        std::is_nothrow_constructible<
            decltype(base::value_storage()),
            std::in_place_index_t<Idx>,
            Args&&...>::value
    ) {
        (construct_at
        )(static_cast<base*>(this), std::in_place_index<Idx>, std::forward<Args>(args)...);
    }

    template <std::size_t CurIdx, std::size_t TargetIdx, class... Args>
    CONSTEXPR20 void emplace_alt(Args&&... args);

    template <class Tagged>
    CONSTEXPR17 void clear(Tagged) noexcept {
        clear<Tagged::index>();
    }

    template <std::size_t Idx>
    CONSTEXPR17 void clear() noexcept {
        if constexpr (!is_valueless<Idx, variant_storage>::value) {
            base::template destroy_union_alt<Idx>();
        }
    }

    CONSTEXPR17 void clear() noexcept {
        if constexpr (!std::conjunction<
                          typename AltStorages::is_trivially_destructible...>::value) {
            tagged_visit([this](auto t) { clear(t); }, *this);
        }
    }

    template <class Tagged>
    CONSTEXPR17 void clear_to_valueless(Tagged) noexcept {
        static_assert(
            !variant_storage::no_valueless_state,
            "variant_storage cannot be valueless, please consider using clear()."
        );
        clear_to_valueless<Tagged::index>();
    }

    template <std::size_t Idx>
    CONSTEXPR17 void clear_to_valueless() noexcept {
        static_assert(
            !variant_storage::no_valueless_state,
            "variant_storage cannot be valueless, please consider using clear()."
        );
        clear<Idx>();
        base::template set_index<base::valueless_index>();
    }

    CONSTEXPR17 void clear_to_valueless() noexcept {
        static_assert(
            !variant_storage::no_valueless_state,
            "variant_storage cannot be valueless, please consider using clear()."
        );
        if constexpr (!std::conjunction<
                          typename AltStorages::is_trivially_destructible...>::value) {
            tagged_visit([this](auto t) { clear(t); }, *this);
        }
        base::set_valueless_index();
    }
};

/// Checks if the given variant_storage supports the FALL_BACK policy. Currently this requires that
/// its first member must be nothrow default-constructible. This can later be changed to fall back
/// to the first nothrow default-constructible member.
template <class... AltStorages>
struct can_fall_back : std::false_type { };

template <class HeadAltStorage, class... TailAltStorages>
struct can_fall_back<HeadAltStorage, TailAltStorages...> :
    HeadAltStorage::is_nothrow_default_constructible { };

template <class Storage>
struct variant_fall_back_guard {
    Storage* target;

    CONSTEXPR20 ~variant_fall_back_guard() {
        if (target) {
            target->template construct_union_alt<0>();
        }
    }
};

template <variant_storage_status Status, class... AltStorages>
template <std::size_t CurIdx, std::size_t TargetIdx, class... Args>
CONSTEXPR20 void variant_storage<Status, AltStorages...>::emplace_alt(Args&&... args) {
    using variant_type = variant<typename AltStorages::value_type...>;
    using target_type = variant_alternative_t<TargetIdx, variant_type>;
    using storage_type = variant_alternative_storage<target_type>;

    if constexpr (std::is_nothrow_constructible<storage_type, Args&&...>::value) {
        // We must test the property of variant_alternative_storage here, because Args can also be
        // variant_alternative_storage.
        clear<CurIdx>();
        construct_union_alt<TargetIdx>(std::forward<Args>(args)...);
    } else if constexpr (storage_type::is_nothrow_move_constructible::value) {
        // We must test the property of the actual value_type of variant_alternative_storage here,
        // see the comment of variant_assign_visitor_impl.
        static_assert(
            std::is_nothrow_move_constructible<storage_type>::value,
            "Internal error: the value stored in "
            "variant_alternative_storage is nothrow-move-constructible, "
            "while variant_alternative_storage itself is not."
        );
        storage_type _temp(std::forward<Args>(args)...);
        clear<CurIdx>();
        construct_union_alt<TargetIdx>(std::move(_temp));
    } else {
        constexpr auto strategy = variant_valueless_strategy<variant_type>::value;
        if constexpr (strategy == variant_valueless_strategy_t::FALLBACK) {
            static_assert(
                can_fall_back<AltStorages...>::value,
                "When using the FALL_BACK strategy, the first member of the variant must be "
                "default-constructible."
            );

            clear<CurIdx>();
            variant_fall_back_guard<
                typename std::remove_reference<decltype(this->storage())>::type>
                _guard { &this->storage() };
            construct_union_alt<TargetIdx>(std::forward<Args>(args)...);
            _guard.target = nullptr;
        } else {
            if constexpr (strategy == variant_valueless_strategy_t::LET_VARIANT_DECIDE) {
                clear_to_valueless<CurIdx>();
            } else {
                clear<CurIdx>();
            }
            construct_union_alt<TargetIdx>(std::forward<Args>(args)...);
        }
    }
}

template <class... AltStorages>
using variant_storage_selector = variant_storage<
    niche_filling<AltStorages...>::has_niche_alternative ? variant_storage_status::NICHE_FILLING
                                                         : variant_storage_status::NORMAL,
    AltStorages...>;
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_VARIANT_STORAGE_HPP
