#ifndef RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_TUPLE_CAT_HPP
#define RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_TUPLE_CAT_HPP

#include <utility>

#include <rust_enum/tuple/detail/fwd/tuple_fwd.hpp>
#include <rust_enum/tuple/detail/universal_get.hpp>
#include <rust_enum/tuple/tuple_element.hpp>
#include <rust_enum/tuple/tuple_size.hpp>
#include <rust_enum/variant/detail/macro.hpp>
#include <rust_enum/variant/detail/std_impl.hpp>

namespace rust::detail {
template <class TTuple, class TupleSeq, class ElemSeq>
struct tuple_cat_result;

template <class TTuple, std::size_t... TIs, std::size_t... EIs>
struct tuple_cat_result<TTuple, std::index_sequence<TIs...>, std::index_sequence<EIs...>> {
    using type = tuple<std::tuple_element_t<EIs, std::tuple_element_t<TIs, TTuple>>...>;
};

template <std::size_t CurIdx, class TupleSeq, class ElemSeq, class... IndexSeqs>
struct tuple_cat_helper {
    using tuple_index_sequence = TupleSeq;
    using element_index_sequence = ElemSeq;

    template <class TTuple>
    using return_type = typename tuple_cat_result<TTuple, TupleSeq, ElemSeq>::type;
};

template <
    std::size_t CurIdx,
    std::size_t... TIs,
    std::size_t... EIs,
    std::size_t... CIs,
    class... Others>
struct tuple_cat_helper<
    CurIdx,
    std::index_sequence<TIs...>,
    std::index_sequence<EIs...>,
    std::index_sequence<CIs...>,
    Others...> :
    tuple_cat_helper<
        CurIdx + 1,
        std::index_sequence<TIs..., (CIs, CurIdx)...>,
        std::index_sequence<EIs..., CIs...>,
        Others...> { };

template <class... Tuples>
using tuple_cat_helper_of = tuple_cat_helper<
    0,
    std::index_sequence<>,
    std::index_sequence<>,
    std::make_index_sequence<std::tuple_size_v<typename remove_cvref<Tuples>::type>>...>;

template <class... Tuples>
using tuple_cat_result_of = typename tuple_cat_helper_of<Tuples...>::template return_type<
    tuple<typename remove_cvref<Tuples>::type...>>;

template <class Ret, class TTuple, std::size_t... TIs, std::size_t... EIs>
CONSTEXPR14 auto tuple_cat_impl(TTuple t, std::index_sequence<TIs...>, std::index_sequence<EIs...>)
    -> Ret {
    return Ret(universal_get<EIs>(rust::get<TIs>(std::move(t)))...);
}
}  // namespace rust::detail

#endif  // RUST_ENUM_INCLUDE_RUST_ENUM_TUPLE_DETAIL_TUPLE_CAT_HPP
