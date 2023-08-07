#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_VARIANT_STORAGE_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_VARIANT_STORAGE_HPP

#include <cstddef>
#include <limits>
#include <type_traits>
#include <utility>

#include <rust_enum/variant/detail/destructible_uninitialized_union.hpp>
#include <rust_enum/variant/detail/fwd/tagged_visit_fwd.hpp>
#include <rust_enum/variant/detail/fwd/variant_storage_fwd.hpp>
#include <rust_enum/variant/detail/macro.hpp>
#include <rust_enum/variant/detail/valueless.hpp>
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

template <class... AltStorages>
struct variant_storage {
    using storage_t = variant_destructible_uninitialized_union_t<AltStorages...>;
    storage_t _value_storage;

    static constexpr bool no_valueless_state =
        variant_no_valueless_state<variant<typename AltStorages::value_type...>>::value;
    // If the current variant will not be valueless, no storage is required for
    // it.
    // FIXME: Since the template parameter of variant may be modified in the
    //  future, we must manually construct the variant type here.
    static constexpr std::size_t status_count =
        no_valueless_state ? sizeof...(AltStorages) : sizeof...(AltStorages) + 1;
    using variant_index_t = typename variant_index<status_count>::type;
    variant_index_t _raw_index;
    // cannot use variant_index_t, which may cause overflow
    static constexpr std::size_t valueless_raw_index = sizeof...(AltStorages);

    CONSTEXPR17 variant_storage() : _value_storage(), _raw_index(0) { }

    template <std::size_t Idx, class... Args>
    CONSTEXPR17 explicit variant_storage(std::in_place_index_t<Idx> _tag, Args&&... args) noexcept(
        std::is_nothrow_constructible<storage_t, std::in_place_index_t<Idx>, Args&&...>::value
    ) :
        _value_storage(_tag, std::forward<Args>(args)...), _raw_index(0) {
        set_index(Idx);
    }

    [[nodiscard]] CONSTEXPR17 auto raw_index() const noexcept -> std::size_t {
        return static_cast<std::size_t>(_raw_index);
    }
    [[nodiscard]] CONSTEXPR17 auto index() const noexcept -> std::size_t {
        // Before implementing niche optimization, raw_index and index are the same.
        return raw_index();
    }
    CONSTEXPR17 void set_raw_index(std::size_t raw_index) noexcept {
        _raw_index = static_cast<variant_index_t>(raw_index);
    }
    CONSTEXPR17 void set_index(std::size_t index) noexcept {
        // Before implementing niche optimization, raw_index and index are the same.
        set_raw_index(index);
    }

    [[nodiscard]] CONSTEXPR17 auto valueless_by_exception() const noexcept -> bool {
        if constexpr (no_valueless_state)
            return false;
        else
            return raw_index() == valueless_raw_index;
    }

    CONSTEXPR17 auto value_storage() & noexcept -> storage_t& {
        return _value_storage;
    }
    CONSTEXPR17 auto value_storage() const& noexcept -> const storage_t& {
        return _value_storage;
    }
    CONSTEXPR17 auto value_storage() && noexcept -> storage_t&& {
        return std::move(_value_storage);
    }
    CONSTEXPR17 auto value_storage() const&& noexcept -> const storage_t&& {
        return std::move(_value_storage);
    }

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

    template <std::size_t Idx, class... Args>
    CONSTEXPR20 void construct_union_alt(Args&&... args) noexcept(
        std::is_nothrow_constructible<storage_t, std::in_place_index_t<Idx>, Args&&...>::value
    ) {
        (construct_at
        )(std::addressof(_value_storage), std::in_place_index<Idx>, std::forward<Args>(args)...);
        set_index(Idx);
    }

    template <std::size_t CurIdx, std::size_t TargetIdx, class... Args>
    CONSTEXPR20 void emplace_alt(Args&&... args);

    template <std::size_t Idx>
    CONSTEXPR20 void destroy_union_alt() noexcept {
        _value_storage.template destroy_content<Idx>();
    }

    template <class Tagged>
    CONSTEXPR17 void clear(Tagged) noexcept {
        clear<Tagged::index>();
    }

    template <std::size_t Idx>
    CONSTEXPR17 void clear() noexcept {
        if constexpr (!is_valueless<Idx, variant_storage>::value) {
            destroy_union_alt<Idx>();
        }
    }

    CONSTEXPR17 void clear() noexcept {
        if constexpr (!std::conjunction<
                          typename AltStorages::is_trivially_destructible...>::value) {
            tagged_visit([this](auto t) { this->clear(t); }, *this);
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
        set_raw_index(valueless_raw_index);
    }

    CONSTEXPR17 void clear_to_valueless() noexcept {
        static_assert(
            !variant_storage::no_valueless_state,
            "variant_storage cannot be valueless, please consider using clear()."
        );
        if constexpr (!std::conjunction<
                          typename AltStorages::is_trivially_destructible...>::value) {
            tagged_visit([this](auto t) { this->clear(t); }, *this);
        }
        set_raw_index(valueless_raw_index);
    }
};

/**
 * Checks if the given variant_storage supports the FALL_BACK policy.
 * Currently this requires that its first member must be
 * nothrow default-constructible. This can later be changed to fall back to
 * the first nothrow default-constructible member.
 */
template <class Storage>
struct can_fall_back : std::false_type { };

template <class HeadAltStorage, class... TailAltStorages>
struct can_fall_back<variant_storage<HeadAltStorage, TailAltStorages...>> :
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

template <class... AltStorages>
template <std::size_t CurIdx, std::size_t TargetIdx, class... Args>
CONSTEXPR20 void variant_storage<AltStorages...>::emplace_alt(Args&&... args) {
    using variant_type = variant<typename AltStorages::value_type...>;
    using target_type = variant_alternative_t<TargetIdx, variant_type>;
    using storage_type = variant_alternative_storage<target_type>;

    if constexpr (std::is_nothrow_constructible<storage_type, Args&&...>::value) {
        // We must test the property of variant_alternative_storage here, because
        // Args can also be variant_alternative_storage.
        this->template clear<CurIdx>();
        this->template construct_union_alt<TargetIdx>(std::forward<Args>(args)...);
    } else if constexpr (storage_type::is_nothrow_move_constructible::value) {
        // We must test the property of the actual value_type of
        // variant_alternative_storage here, see the comment of
        // variant_assign_visitor_impl.
        static_assert(
            std::is_nothrow_move_constructible<storage_type>::value,
            "Internal error: the value stored in "
            "variant_alternative_storage is nothrow-move-constructible, "
            "while variant_alternative_storage itself is not."
        );
        storage_type _temp(std::forward<Args>(args)...);
        this->template clear<CurIdx>();
        this->template construct_union_alt<TargetIdx>(std::move(_temp));
    } else {
        if constexpr (variant_valueless_strategy<variant_type>::value == variant_valueless_strategy_t::FALLBACK) {
            static_assert(
                can_fall_back<variant_storage<AltStorages...>>::value,
                "When using the FALL_BACK strategy, the first member of "
                "the variant must be default-constructible."
            );

            this->template clear<CurIdx>();
            variant_fall_back_guard<
                typename std::remove_reference<decltype(this->storage())>::type>
                _guard { &this->storage() };
            this->template construct_union_alt<TargetIdx>(std::forward<Args>(args)...);
            _guard.target = nullptr;
        } else {
            if constexpr (variant_valueless_strategy<variant_type>::value == variant_valueless_strategy_t::LET_VARIANT_DECIDE) {
                this->template clear_to_valueless<CurIdx>();
            } else {
                this->template clear<CurIdx>();
            }
            this->template construct_union_alt<TargetIdx>(std::forward<Args>(args)...);
        }
    }
}
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_VARIANT_STORAGE_HPP
