#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_TAGGED_VISIT_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_TAGGED_VISIT_HPP

#include <cstddef>
#include <utility>

#include <rust_enum/variant/bad_variant_access.hpp>
#include <rust_enum/variant/detail/cartesian_product.hpp>
#include <rust_enum/variant/detail/fwd/tagged_visit_fwd.hpp>
#include <rust_enum/variant/detail/fwd/variant_storage_fwd.hpp>
#include <rust_enum/variant/detail/macro.hpp>
#include <rust_enum/variant/detail/std_impl.hpp>
#include <rust_enum/variant/detail/valueless.hpp>

namespace rust::detail {
template <class>
struct variant_visit_dispatcher;

template <std::size_t... Is>
struct variant_visit_dispatcher<std::index_sequence<Is...>> {
    template <class... Storages>
    struct is_valid_apply_storage :
        std::conjunction<
            is_specialization_of_variant_storage<typename remove_cvref<Storages>::type>...> { };

    template <
        class Ret,
        class Fn,
        class... Storages,
        typename std::enable_if<is_valid_apply_storage<Storages...>::value, int>::type = 0>
    static CONSTEXPR17 auto tagged_apply_r(Fn&& func, Storages&&... storages) -> Ret {
        if constexpr (std::is_void<Ret>::value) {
            return static_cast<void>(std::invoke(
                std::forward<Fn>(func),
                get_variant_tagged_content<Is>(std::forward<Storages>(storages))...
            ));
        } else {
            // Implicit conversion.
            return std::invoke(
                std::forward<Fn>(func),
                get_variant_tagged_content<Is>(std::forward<Storages>(storages))...
            );
        }
    }

    template <class... Storages>
    struct any_valueless : std::disjunction<is_valueless<Is, Storages>...> { };

    template <
        class Ret,
        class Fn,
        class... Storages,
        typename std::enable_if<is_valid_apply_storage<Storages...>::value, int>::type = 0>
    static CONSTEXPR17 auto apply_r(Fn&& func, Storages&&... storages) -> Ret {
        if constexpr (any_valueless<typename std::remove_reference<Storages>::type...>::value) {
            throw_bad_variant_access();
        } else if constexpr (std::is_void<Ret>::value) {
            return static_cast<void>(std::invoke(
                std::forward<Fn>(func),
                get_variant_tagged_content<Is>(std::forward<Storages>(storages))
                    .forward_content()
                    .forward_value()...
            ));
        } else {
            // Implicit conversion.
            return std::invoke(
                std::forward<Fn>(func),
                get_variant_tagged_content<Is>(std::forward<Storages>(storages))
                    .forward_content()
                    .forward_value()...
            );
        }
    }
};

template <class IndexSequence, bool Tagged>
struct tagged_dispatcher_caller {
    template <class Ret, class Fn, class... Storages>
    static CONSTEXPR17 auto call(Fn&& func, Storages&&... storages) -> Ret {
        return variant_visit_dispatcher<IndexSequence>::template apply_r<Ret>(
            std::forward<Fn>(func),
            std::forward<Storages>(storages)...
        );
    }
};

template <std::size_t... Is>
struct tagged_dispatcher_caller<std::index_sequence<Is...>, true> {
    template <class Ret, class Fn, class... Storages>
    static CONSTEXPR17 auto call(Fn&& func, Storages&&... storages) -> Ret {
        return variant_visit_dispatcher<std::index_sequence<Is...>>::template tagged_apply_r<Ret>(
            std::forward<Fn>(func),
            std::forward<Storages>(storages)...
        );
    }
};

template <bool Tagged>
struct tagged_dispatcher_caller<index_sequence_list_iterator_end, Tagged> {
    template <class Ret, class Fn, class... Storages>
    [[noreturn]] static CONSTEXPR17 auto call(Fn&&, Storages&&...) -> Ret {
        unreachable();
    }
};

template <int strategy>
struct visit_strategy {
    template <bool Tagged, class Ret, class Fn, class IndexSequenceList, class... Storages>
    struct dispatcher_table;

    template <bool Tagged, class Ret, class Fn, class... IndexSequences, class... Storages>
    struct dispatcher_table<Tagged, Ret, Fn, index_sequence_list<IndexSequences...>, Storages...> {
        using dispatcher = Ret (*)(Fn&&, Storages&&...);
        static constexpr dispatcher table[] = {
            &(tagged_dispatcher_caller<IndexSequences, Tagged>::
                  template call<Ret, Fn, Storages...>) ...
        };
    };

    template <class Ret, class IndexSequenceList, bool Tagged, class Fn, class... Storages>
    static CONSTEXPR17 auto visit(  //
        std::size_t global_index,
        Fn&& func,
        Storages&&... storages
    ) -> Ret {
        constexpr auto& table =
            dispatcher_table<Tagged, Ret, Fn, IndexSequenceList, Storages...>::table;
        return table[global_index](std::forward<Fn>(func), std::forward<Storages>(storages)...);
    }
};

#define VISIT_CASE(base, index, caller, last_iterator, next_iterator)                             \
case (base) + (index):                                                                            \
    return caller<typename last_iterator::type, Tagged>::template call<Ret>(                      \
        std::forward<Fn>(func),                                                                   \
        std::forward<Storages>(storages)...                                                       \
    );                                                                                            \
    using next_iterator [[maybe_unused]] = typename last_iterator::next

#define VISIT_4CASES(base, caller, last_prefix, prefix)                                           \
    VISIT_CASE(base, 0, caller, last_prefix##3, prefix##0);                                       \
    VISIT_CASE(base, 1, caller, prefix##0, prefix##1);                                            \
    VISIT_CASE(base, 2, caller, prefix##1, prefix##2);                                            \
    VISIT_CASE(base, 3, caller, prefix##2, prefix##3)

#define VISIT_16CASES(base, caller, last_prefix, prefix)                                          \
    VISIT_4CASES(base, caller, last_prefix##3, prefix##0);                                        \
    VISIT_4CASES(base + 4, caller, prefix##0, prefix##1);                                         \
    VISIT_4CASES(base + 8, caller, prefix##1, prefix##2);                                         \
    VISIT_4CASES(base + 12, caller, prefix##2, prefix##3)

#define VISIT_64CASES(base, caller, last_prefix, prefix)                                          \
    VISIT_16CASES(base, caller, last_prefix##3, prefix##0);                                       \
    VISIT_16CASES(base + 16, caller, prefix##0, prefix##1);                                       \
    VISIT_16CASES(base + 32, caller, prefix##1, prefix##2);                                       \
    VISIT_16CASES(base + 48, caller, prefix##2, prefix##3)

#define VISIT_256CASES(base, caller, last_prefix, prefix)                                         \
    VISIT_64CASES(base, caller, last_prefix##3, prefix##0);                                       \
    VISIT_64CASES(base + 64, caller, prefix##0, prefix##1);                                       \
    VISIT_64CASES(base + 128, caller, prefix##1, prefix##2);                                      \
    VISIT_64CASES(base + 192, caller, prefix##2, prefix##3)

#define VISIT_BODY(suffix, macro)                                                                 \
    using _begin##suffix = index_sequence_list_iterator<IndexSequenceList>;                       \
    switch (global_index) {                                                                       \
        macro(0, tagged_dispatcher_caller, _begin, _iterator);                                    \
    default:                                                                                      \
        return tagged_dispatcher_caller<index_sequence_list_iterator_end, Tagged>::template call< \
            Ret>(std::forward<Fn>(func), std::forward<Storages>(storages)...);                    \
    }

template <>
struct visit_strategy<1> {
    template <class Ret, class IndexSequenceList, bool Tagged, class Fn, class... Storages>
    static CONSTEXPR17 auto visit(  //
        std::size_t global_index,
        Fn&& func,
        Storages&&... storages
    ) -> Ret {
        VISIT_BODY(3, VISIT_4CASES)
    }
};

template <>
struct visit_strategy<2> {
    template <class Ret, class IndexSequenceList, bool Tagged, class Fn, class... Storages>
    static CONSTEXPR17 auto visit(  //
        std::size_t global_index,
        Fn&& func,
        Storages&&... storages
    ) -> Ret {
        VISIT_BODY(33, VISIT_16CASES)
    }
};

template <>
struct visit_strategy<3> {
    template <class Ret, class IndexSequenceList, bool Tagged, class Fn, class... Storages>
    static CONSTEXPR17 auto visit(  //
        std::size_t global_index,
        Fn&& func,
        Storages&&... storages
    ) -> Ret {
        VISIT_BODY(333, VISIT_64CASES)
    }
};

template <>
struct visit_strategy<4> {
    template <class Ret, class IndexSequenceList, bool Tagged, class Fn, class... Storages>
    static CONSTEXPR17 auto visit(  //
        std::size_t global_index,
        Fn&& func,
        Storages&&... storages
    ) -> Ret {
        VISIT_BODY(3333, VISIT_256CASES)
    }
};

#undef VISIT_BODY
#undef VISIT_256CASES
#undef VISIT_64CASES
#undef VISIT_16CASES
#undef VISIT_4CASES

template <std::size_t TotalSize>
struct visit_strategy_selector :
    std::conditional<
        TotalSize <= 4,
        visit_strategy<1>,
        typename std::conditional<
            TotalSize <= 16,
            visit_strategy<2>,
            typename std::conditional<
                TotalSize <= 64,
                visit_strategy<3>,
                typename std::
                    conditional<TotalSize <= 256, visit_strategy<4>, visit_strategy<-1>>::type>::
                type>::type> { };

template <class Ret, bool Tagged, class Fn, class... Storages>
CONSTEXPR17 auto visit_impl(Fn&& func, Storages&&... storages) -> Ret {
    constexpr std::size_t total_status_count =
        (std::size_t(1) * ... * std::remove_reference<Storages>::type::status_count);
    static_assert(total_status_count != 0, "total_status_count cannot be 0.");

    using strategy = typename visit_strategy_selector<total_status_count>::type;

    std::size_t global_index = 0;
    std::size_t _unused[] = { 0, /* For empty `storages`. */
                              (global_index = global_index
                                       * std::remove_reference<Storages>::type::status_count
                                   + storages.index())... };
    (void) _unused;

    return strategy::template visit<
        Ret,
        typename status_index_cartesian_product<
            std::remove_reference<Storages>::type::status_count...>::type,
        Tagged>(global_index, std::forward<Fn>(func), std::forward<Storages>(storages)...);
}

template <class Fn, class... Storages>
CONSTEXPR17 auto tagged_visit(  //
    Fn&& func,
    Storages&&... storages
) -> typename tagged_apply_result<Fn, Storages...>::type {
    using ret_type = typename tagged_apply_result<Fn, Storages...>::type;
    return visit_impl<ret_type, true>(std::forward<Fn>(func), std::forward<Storages>(storages)...);
}
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_VARIANT_DETAIL_TAGGED_VISIT_HPP
