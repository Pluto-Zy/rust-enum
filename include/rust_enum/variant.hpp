#ifndef RUST_ENUM_VARIANT_HPP
#define RUST_ENUM_VARIANT_HPP

#include <array>
#include <cstdint>
#include <exception>
#include <functional>
#include <initializer_list>
#include <limits>
#include <memory>
#include <tuple>
#include <type_traits>

#if __cplusplus >= 201103L
  #define USE_CXX11
#endif
#if __cplusplus >= 201402L
  #define USE_CXX14
#endif
#if __cplusplus >= 201703L
  #define USE_CXX17
#endif
#if __cplusplus >= 202002L
  #define USE_CXX20
#endif

#ifdef USE_CXX11
  #define CONSTEXPR11 constexpr
#else
  #define CONSTEXPR11 inline
#endif

#ifdef USE_CXX14
  #define CONSTEXPR14 constexpr
#else
  #define CONSTEXPR14 inline
#endif

#ifdef USE_CXX17
  #define CONSTEXPR17 constexpr
#else
  #define CONSTEXPR17 inline
#endif

#ifdef USE_CXX20
  #define CONSTEXPR20 constexpr
  #define CXX20_DEPRECATE_VOLATILE                                             \
    [[deprecated("Some operations on volatile-qualified types in the STL are " \
                 "deprecated in C++20.")]]
#else
  #define CONSTEXPR20 inline
  #define CXX20_DEPRECATE_VOLATILE
#endif

namespace rust {

template <class...>
class variant;

template <std::size_t Idx, class Ty>
struct variant_alternative;

// use std::tuple_element to implement it currently
template <std::size_t Idx, class... Tys>
struct variant_alternative<Idx, variant<Tys...>>
    : std::tuple_element<Idx, std::tuple<Tys...>> { };

template <std::size_t Idx, class Ty>
using variant_alternative_t = typename variant_alternative<Idx, Ty>::type;

template <std::size_t Idx, class Ty>
struct variant_alternative<Idx, const Ty>
    : std::add_const<variant_alternative_t<Idx, Ty>> { };

template <std::size_t Idx, class Ty>
struct CXX20_DEPRECATE_VOLATILE variant_alternative<Idx, volatile Ty>
    : std::add_volatile<variant_alternative_t<Idx, Ty>> { };

template <std::size_t Idx, class Ty>
struct CXX20_DEPRECATE_VOLATILE variant_alternative<Idx, const volatile Ty>
    : std::add_cv<variant_alternative_t<Idx, Ty>> { };

template <class Ty>
struct variant_size;

template <class... Tys>
struct variant_size<variant<Tys...>>
    : std::integral_constant<std::size_t, sizeof...(Tys)> { };

template <class Ty>
struct variant_size<const Ty> : variant_size<Ty> { };

template <class Ty>
struct CXX20_DEPRECATE_VOLATILE variant_size<volatile Ty> : variant_size<Ty> {
};

template <class Ty>
struct CXX20_DEPRECATE_VOLATILE variant_size<const volatile Ty>
    : variant_size<Ty> { };

template <class Ty>
inline constexpr std::size_t variant_size_v = variant_size<Ty>::value;

/**
 * Defines the processing strategy when the variant will become valueless.
 */
enum class variant_valueless_strategy_t {
  /**
   * If it was possible to get rid of valueless state while providing strong
   * exception-safety guarantee, it would do so.
   */
  LET_VARIANT_DECIDE,
  /**
   * Disallow variants from being valueless. If the variant will never becomes
   * valueless (for example, has a non-throwing move assignment operator), the
   * variant still provides the strong exception-safety guarantee. Otherwise, in
   * order to get rid of the valueless state, no guarantees will be provided.
   */
  DISALLOW_VALUELESS,
  /**
   * When going to be valueless, fall back to the first member. This requires
   * the first member to be default-constructible. This applies to optional:
   * when an exception is thrown during construction, fall back to the bool
   * member.
   */
  FALLBACK,
};

template <class Variant>
struct variant_valueless_strategy {
  static constexpr variant_valueless_strategy_t value =
      variant_valueless_strategy_t::LET_VARIANT_DECIDE;
};

// Checks whether a variant must not contain valueless state.
template <class Variant, variant_valueless_strategy_t strategy =
                             variant_valueless_strategy<Variant>::value>
struct variant_no_valueless_state;

template <class... Tys, variant_valueless_strategy_t strategy>
struct variant_no_valueless_state<variant<Tys...>, strategy>
    : std::disjunction<
          std::bool_constant<strategy ==
                             variant_valueless_strategy_t::DISALLOW_VALUELESS>,
          std::bool_constant<strategy ==
                             variant_valueless_strategy_t::FALLBACK>,
          std::conjunction<std::is_nothrow_move_constructible<Tys>...>> { };

// std::construct_at() is constexpr in C++20
#ifdef USE_CXX20
using std::construct_at;
using std::destroy_at;
#else
template <class Ty, class... Args>
Ty* construct_at(Ty* ptr, Args&&... args) {
  return ::new (const_cast<void*>(static_cast<const volatile void*>(ptr)))
      Ty(std::forward<Args>(args)...);
}

template <class Ty>
void destroy_at(Ty* location) noexcept {
  location->~Ty();
}
#endif  // USE_CXX20

template <bool IsTriviallyDestructible, class... Tys>
union variant_destructible_uninitialized_union {
  /* For empty Tys. */
};

template <class... Tys>
using variant_destructible_uninitialized_union_t =
    variant_destructible_uninitialized_union<
        std::conjunction<std::is_trivially_destructible<Tys>...>::value,
        Tys...>;

// clang-format off
#define DESTRUCTIBLE_UNINITIALIZED_UNION_TEMPLATE(is_trivially_destructible,   \
                                                  destructor)                  \
  template <class Head, class... Tail>                                         \
  union variant_destructible_uninitialized_union<is_trivially_destructible,    \
                                                 Head, Tail...> {              \
    /* According to the standard, for a non-empty union, its constexpr         \
     * constructor must initialize exactly one variant member. To satisfy this \
     * requirement, the constexpr constructor of                               \
     * variant_destructible_uninitialized_union will initialize the _dummy     \
     * member. _dummy will be the active member of the union. The reason tail  \
     * is not initialized is that tail's destructor can be non-trivial, which  \
     * causes us to need to explicitly call tail's destructor to make it       \
     * inactive. (Although its destructor doesn't do anything.)                \
     */                                                                        \
    unsigned char _dummy;                                                      \
    typename std::remove_const<Head>::type value;                              \
    variant_destructible_uninitialized_union_t<Tail...> tail;                  \
                                                                               \
    CONSTEXPR17 variant_destructible_uninitialized_union() noexcept :          \
        _dummy() { }                                                           \
                                                                               \
    template <std::size_t Idx, class... Args,                                  \
              typename std::enable_if<(Idx > 0), int>::type = 0>               \
    CONSTEXPR17 explicit variant_destructible_uninitialized_union(             \
        std::in_place_index_t<Idx>, Args&&... args) noexcept(                  \
            std::is_nothrow_constructible<decltype(tail),                      \
                                          std::in_place_index_t<Idx - 1>,      \
                                          Args&&...>::value) :                 \
        tail(std::in_place_index<Idx - 1>, std::forward<Args>(args)...) { }    \
                                                                               \
    template <class... Args>                                                   \
    CONSTEXPR17 explicit variant_destructible_uninitialized_union(             \
        std::in_place_index_t<0>, Args&&... args) noexcept(                    \
            std::is_nothrow_constructible<Head, Args&&...>::value) :           \
        value(std::forward<Args>(args)...) { }                                 \
                                                                               \
    /* Defaulted copy/move constructor/assignment will be constexpr/noexcept   \
     * if possible. */                                                         \
    variant_destructible_uninitialized_union(                                  \
        const variant_destructible_uninitialized_union&) = default;            \
                                                                               \
    variant_destructible_uninitialized_union(                                  \
        variant_destructible_uninitialized_union&&) = default;                 \
                                                                               \
    variant_destructible_uninitialized_union&                                  \
    operator=(const variant_destructible_uninitialized_union&) = default;      \
                                                                               \
    variant_destructible_uninitialized_union&                                  \
    operator=(variant_destructible_uninitialized_union&&) = default;           \
                                                                               \
    template <std::size_t Idx>                                                 \
    CONSTEXPR20 void destroy_content() noexcept {                              \
      if constexpr (Idx > 0) {                                                 \
        tail.template destroy_content<Idx - 1>();                              \
        (destroy_at)(&tail);                                                   \
      } else {                                                                 \
        (destroy_at)(&value);                                                  \
      }                                                                        \
    }                                                                          \
                                                                               \
    destructor                                                                 \
  };
// clang-format on

DESTRUCTIBLE_UNINITIALIZED_UNION_TEMPLATE(
    true, CONSTEXPR20 ~variant_destructible_uninitialized_union() = default;)

DESTRUCTIBLE_UNINITIALIZED_UNION_TEMPLATE(
    false, CONSTEXPR20 ~variant_destructible_uninitialized_union() {
               /* non-trivial */
           })

#undef DESTRUCTIBLE_UNINITIALIZED_UNION_TEMPLATE

template <std::size_t StatusCount>
struct variant_index
    : std::conditional<
          StatusCount <= (std::numeric_limits<std::uint8_t>::max)(),
          std::uint8_t,
          typename std::conditional<
              StatusCount <= (std::numeric_limits<std::uint16_t>::max)(),
              std::uint16_t, std::uint32_t>::type> { };

template <class... Tys>
struct variant_storage {
  using storage_t = variant_destructible_uninitialized_union_t<Tys...>;
  storage_t _value_storage;

  static constexpr bool no_valueless_state =
      variant_no_valueless_state<variant<Tys...>>::value;
  // If the current variant will not be valueless, no storage is required for
  // it.
  // FIXME: Since the template parameter of variant may be modified in the
  //  future, we must manually construct the variant type here.
  static constexpr std::size_t status_count =
      no_valueless_state ? sizeof...(Tys) : sizeof...(Tys) + 1;
  using variant_index_t = typename variant_index<status_count>::type;
  variant_index_t _raw_index;
  // cannot use variant_index_t, which may cause overflow
  static constexpr std::size_t valueless_raw_index = sizeof...(Tys);

  CONSTEXPR17 variant_storage() : _value_storage(), _raw_index(0) { }

  template <std::size_t Idx, class... Args>
  CONSTEXPR17 explicit variant_storage(std::in_place_index_t<Idx> _tag,
                                       Args&&... args) noexcept(  //
      std::is_nothrow_constructible<storage_t, std::in_place_index_t<Idx>,
                                    Args&&...>::value) :
      _value_storage(_tag, std::forward<Args>(args)...),
      _raw_index(0) {
    set_index(Idx);
  }

  variant_storage(const variant_storage&) = default;
  variant_storage(variant_storage&&) = default;
  variant_storage& operator=(const variant_storage&) = default;
  variant_storage& operator=(variant_storage&&) = default;

  [[nodiscard]] CONSTEXPR17 std::size_t raw_index() const noexcept {
    return static_cast<std::size_t>(_raw_index);
  }
  [[nodiscard]] CONSTEXPR17 std::size_t index() const noexcept {
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

  [[nodiscard]] CONSTEXPR17 bool valueless_by_exception() const noexcept {
    if constexpr (no_valueless_state)
      return false;
    else
      return raw_index() == valueless_raw_index;
  }

  CONSTEXPR17 storage_t& value_storage() & noexcept { return _value_storage; }
  CONSTEXPR17 const storage_t& value_storage() const& noexcept {
    return _value_storage;
  }
  CONSTEXPR17 storage_t&& value_storage() && noexcept {
    return std::move(_value_storage);
  }
  CONSTEXPR17 const storage_t&& value_storage() const&& noexcept {
    return std::move(_value_storage);
  }

  CONSTEXPR17 variant_storage& storage() & noexcept { return *this; }
  CONSTEXPR17 const variant_storage& storage() const& noexcept { return *this; }
  CONSTEXPR17 variant_storage&& storage() && noexcept {
    return std::move(*this);
  }
  CONSTEXPR17 const variant_storage&& storage() const&& noexcept {
    return std::move(*this);
  }

  template <std::size_t Idx, class... Args>
  CONSTEXPR20 void construct_union_alt(Args&&... args) noexcept(
      std::is_nothrow_constructible<storage_t, std::in_place_index_t<Idx>,
                                    Args&&...>::value) {
    (construct_at)(std::addressof(_value_storage), std::in_place_index<Idx>,
                   std::forward<Args>(args)...);
    set_index(Idx);
  }

  template <std::size_t CurIdx, std::size_t TargetIdx, class... Args>
  CONSTEXPR20 void emplace_alt(Args&&... args);

  template <std::size_t Idx>
  CONSTEXPR20 void destroy_union_alt() noexcept {
    _value_storage.template destroy_content<Idx>();
  }

  template <class Tagged>
  CONSTEXPR17 void clear(Tagged) noexcept;
  template <std::size_t Idx>
  CONSTEXPR17 void clear() noexcept;
  CONSTEXPR17 void clear() noexcept;

  template <class Tagged>
  CONSTEXPR17 void clear_to_valueless(Tagged) noexcept;
  template <std::size_t Idx>
  CONSTEXPR17 void clear_to_valueless() noexcept;
  CONSTEXPR17 void clear_to_valueless() noexcept;
};

template <std::size_t Idx, class Union>
struct variant_element_helper;

template <std::size_t Idx, class... Tys>
struct variant_element_helper<
    Idx, variant_destructible_uninitialized_union_t<Tys...>&> {
  using raw_type = variant_alternative_t<Idx, variant<Tys...>>;
  using reference_type = raw_type&;
};

template <std::size_t Idx, class... Tys>
struct variant_element_helper<
    Idx, variant_destructible_uninitialized_union_t<Tys...> const&> {
  using raw_type = variant_alternative_t<Idx, const variant<Tys...>>;
  using reference_type = raw_type&;
};

template <std::size_t Idx, class... Tys>
struct variant_element_helper<
    Idx, variant_destructible_uninitialized_union_t<Tys...>&&> {
  using raw_type = variant_alternative_t<Idx, variant<Tys...>>;
  using reference_type = raw_type&&;
};

template <std::size_t Idx, class... Tys>
struct variant_element_helper<
    Idx, variant_destructible_uninitialized_union_t<Tys...> const&&> {
  using raw_type = variant_alternative_t<Idx, const variant<Tys...>>;
  using reference_type = raw_type&&;
};

template <class>
struct is_specialization_of_variant_destructible_uninitialized_union
    : std::false_type { };

template <bool IsTriviallyDestructible, class... Tys>
struct is_specialization_of_variant_destructible_uninitialized_union<
    variant_destructible_uninitialized_union<IsTriviallyDestructible, Tys...>>
    : std::true_type { };

template <class>
struct is_specialization_of_variant_storage : std::false_type { };

template <class... Tys>
struct is_specialization_of_variant_storage<variant_storage<Tys...>>
    : std::true_type { };

#ifdef USE_CXX20
using std::remove_cvref;
#else
// Implementation of C++20 std::remove_cvref
template <class Ty>
struct remove_cvref : std::remove_cv<typename std::remove_reference<Ty>::type> {
};
#endif

/// This function is used to get the pointer to the Idx-th member of the
/// variant. It is functionally equivalent to the following implementation:
///
/// template <std::size_t Idx, class Union,
///           class Trait = typename std::enable_if<
///               is_specialization_of_variant_destructible_uninitialized_union<
///                   typename remove_cvref<Union>::type>::value,
///               variant_element_helper<Idx, Union&&>>::type>
/// typename Trait::reference_type
/// get_variant_content(Union&& _union) noexcept {
///   return static_cast<typename Trait::reference_type>(
///       *reinterpret_cast<typename Trait::storage_type*>(&_union));
/// }
///
/// However, we cannot use `reinterpret_cast` in constexpr function.
template <std::size_t Idx, class Union,
          class Trait = typename std::enable_if<
              is_specialization_of_variant_destructible_uninitialized_union<
                  typename remove_cvref<Union>::type>::value,
              variant_element_helper<Idx, Union&&>>::type>
CONSTEXPR17 typename Trait::reference_type
get_variant_content(Union&& _union) noexcept {
  if constexpr (Idx == 0) {
    return std::forward<Union>(_union).value;
  } else if constexpr (Idx == 1) {
    return std::forward<Union>(_union).tail.value;
  } else if constexpr (Idx == 2) {
    return std::forward<Union>(_union).tail.tail.value;
  } else if constexpr (Idx == 3) {
    return std::forward<Union>(_union).tail.tail.tail.value;
  } else if constexpr (Idx == 4) {
    return std::forward<Union>(_union).tail.tail.tail.tail.value;
  } else if constexpr (Idx == 5) {
    return std::forward<Union>(_union).tail.tail.tail.tail.tail.value;
  } else if constexpr (Idx == 6) {
    return std::forward<Union>(_union).tail.tail.tail.tail.tail.tail.value;
  } else if constexpr (Idx == 7) {
    return std::forward<Union>(_union).tail.tail.tail.tail.tail.tail.tail.value;
  } else {
    return get_variant_content<Idx - 8>(
        std::forward<Union>(_union).tail.tail.tail.tail.tail.tail.tail.tail);
  }
}

struct valueless_tag { };

template <std::size_t Idx, class Ty>
struct tagged_reference {
  static_assert(std::is_reference<Ty>::value, "Ty must be reference.");

  using type = Ty;
  static constexpr std::size_t index = Idx;
  Ty content;

  template <class Self = Ty, typename std::enable_if<
                                 std::is_reference<Self>::value, int>::type = 0>
  CONSTEXPR17 explicit tagged_reference(Ty content) :
      content(std::forward<Ty>(content)) { }

  CONSTEXPR17 tagged_reference(const tagged_reference& other) :
      content(std::forward<Ty>(other.content)) { }

  CONSTEXPR17 Ty forward_content() const { return std::forward<Ty>(content); }
};

template <std::size_t Idx>
struct tagged_reference<Idx, valueless_tag> {
  using type = valueless_tag;
  static constexpr std::size_t index = Idx;

  CONSTEXPR17 valueless_tag forward_content() const { return {}; }
};

// FIXME: Here `Is` is index, but `valueless_raw_index` is raw_index.
template <std::size_t Idx, class Storage>
struct is_valueless
    : std::bool_constant<
          !std::remove_reference<Storage>::type::no_valueless_state &&
          Idx == std::remove_reference<Storage>::type::status_count - 1> { };

template <std::size_t Idx, class Storage,
          bool = is_valueless<Idx, Storage>::value>
struct get_variant_tagged_content_impl {
  static CONSTEXPR17
      tagged_reference<Idx, decltype(get_variant_content<Idx>(
                                std::declval<Storage>().value_storage()))>
      call(Storage&& storage) noexcept {
    return tagged_reference<
        Idx, decltype(get_variant_content<Idx>(
                 std::forward<Storage>(storage).value_storage()))> {
        get_variant_content<Idx>(
            std::forward<Storage>(storage).value_storage())};
  }
};

template <std::size_t Idx, class Storage>
struct get_variant_tagged_content_impl<Idx, Storage, true> {
  static CONSTEXPR17 tagged_reference<Idx, valueless_tag>
  call(Storage&&) noexcept {
    return tagged_reference<Idx, valueless_tag> {};
  }
};

template <std::size_t Idx, class Storage>
CONSTEXPR17 decltype(get_variant_tagged_content_impl<Idx, Storage>::call(
    std::declval<Storage>()))
get_variant_tagged_content(Storage&& storage) noexcept {
  return get_variant_tagged_content_impl<Idx, Storage>::call(
      std::forward<Storage>(storage));
}

class bad_variant_access : public std::exception {
public:
  const char* what() const noexcept override { return "bad variant access"; }
};

[[noreturn]] inline void throw_bad_variant_access() {
  throw bad_variant_access {};
}

template <class>
struct variant_visit_dispatcher;

template <std::size_t... Is>
struct variant_visit_dispatcher<std::index_sequence<Is...>> {
  template <class... Storages>
  struct is_valid_apply_storage
      : std::conjunction<is_specialization_of_variant_storage<
            typename remove_cvref<Storages>::type>...> { };

  template <class Ret, class Fn, class... Storages,
            typename std::enable_if<is_valid_apply_storage<Storages...>::value,
                                    int>::type = 0>
  static CONSTEXPR17 Ret tagged_apply_r(Fn&& func, Storages&&... storages) {
    if constexpr (std::is_void<Ret>::value) {
      return static_cast<void>(std::invoke(
          std::forward<Fn>(func),
          get_variant_tagged_content<Is>(std::forward<Storages>(storages))...));
    } else {
      // Implicit conversion.
      return std::invoke(
          std::forward<Fn>(func),
          get_variant_tagged_content<Is>(std::forward<Storages>(storages))...);
    }
  }

  template <class... Storages>
  struct any_valueless : std::disjunction<is_valueless<Is, Storages>...> { };

  template <class Ret, class Fn, class... Storages,
            typename std::enable_if<is_valid_apply_storage<Storages...>::value,
                                    int>::type = 0>
  static CONSTEXPR17 Ret apply_r(Fn&& func, Storages&&... storages) {
    if constexpr (any_valueless<typename std::remove_reference<
                      Storages>::type...>::value) {
      throw_bad_variant_access();
    } else if constexpr (std::is_void<Ret>::value) {
      return static_cast<void>(std::invoke(
          std::forward<Fn>(func),
          get_variant_tagged_content<Is>(std::forward<Storages>(storages))
              .forward_content()...));
    } else {
      // Implicit conversion.
      return std::invoke(
          std::forward<Fn>(func),
          get_variant_tagged_content<Is>(std::forward<Storages>(storages))
              .forward_content()...);
    }
  }
};

/* The following piece of code produces a Cartesian product of type indices. For
 * example, given variant<T1, T2>, variant<W1, W2, W3>, variant<U1, U2>, when
 * visiting these variants, we will generate 2*3*2=12 combinations of states.
 * The content of these combinations can be obtained through
 * status_index_cartesian_product<2, 3, 2>::type, which will be
 * index_sequence_list<
 * std::index_sequence<0, 0, 0>, std::index_sequence<0, 0, 1>,
 * std::index_sequence<0, 1, 0>, std::index_sequence<0, 1, 1>,
 * std::index_sequence<0, 2, 0>, std::index_sequence<0, 2, 1>,
 * std::index_sequence<1, 0, 0>, std::index_sequence<1, 0, 1>,
 * std::index_sequence<1, 1, 0>, std::index_sequence<1, 1, 1>,
 * std::index_sequence<1, 2, 0>, std::index_sequence<1, 2, 1>>.
 */

template <class...>
struct index_sequence_list { };

template <class...>
struct index_sequence_list_concat;

template <class List>
struct index_sequence_list_concat<List> {
  using type = List;
};

template <class... HeadIndexSequence, class... NextIndexSequence,
          class... TailLists>
struct index_sequence_list_concat<index_sequence_list<HeadIndexSequence...>,
                                  index_sequence_list<NextIndexSequence...>,
                                  TailLists...>
    : index_sequence_list_concat<
          index_sequence_list<HeadIndexSequence..., NextIndexSequence...>,
          TailLists...> { };

template <class...>
struct status_index_cartesian_product_impl {
  using type = index_sequence_list<std::index_sequence<>>;
};

template <std::size_t... Already, std::size_t... Current>
struct status_index_cartesian_product_impl<std::index_sequence<Already...>,
                                           std::index_sequence<Current...>> {
  using type = index_sequence_list<std::index_sequence<Already..., Current>...>;
};

template <std::size_t... Already, std::size_t... Current, class... Others>
struct status_index_cartesian_product_impl<
    std::index_sequence<Already...>, std::index_sequence<Current...>, Others...>
    : index_sequence_list_concat<typename status_index_cartesian_product_impl<
          std::index_sequence<Already..., Current>, Others...>::type...> { };

template <std::size_t... StatusCount>
struct status_index_cartesian_product
    : status_index_cartesian_product_impl<
          std::index_sequence<>, std::make_index_sequence<StatusCount>...> { };

struct index_sequence_list_iterator_end;

template <class List>
struct index_sequence_list_iterator {
  using type = index_sequence_list_iterator_end;
  using next = index_sequence_list_iterator<List>;
};

template <class Head, class... Tails>
struct index_sequence_list_iterator<index_sequence_list<Head, Tails...>> {
  // dereference
  using type = Head;
  using next = index_sequence_list_iterator<index_sequence_list<Tails...>>;
};

template <class IndexSequence, bool Tagged>
struct tagged_dispatcher_caller {
  template <class Ret, class Fn, class... Storages>
  static CONSTEXPR17 Ret call(Fn&& func, Storages&&... storages) {
    return variant_visit_dispatcher<IndexSequence>::template apply_r<Ret>(
        std::forward<Fn>(func), std::forward<Storages>(storages)...);
  }
};

template <std::size_t... Is>
struct tagged_dispatcher_caller<std::index_sequence<Is...>, true> {
  template <class Ret, class Fn, class... Storages>
  static CONSTEXPR17 Ret call(Fn&& func, Storages&&... storages) {
    return variant_visit_dispatcher<std::index_sequence<Is...>>::
        template tagged_apply_r<Ret>(std::forward<Fn>(func),
                                     std::forward<Storages>(storages)...);
  }
};

#ifdef __GNUC__  // GCC 4.8+, Clang, Intel and other compilers compatible with
                 // GCC (-std=c++0x or above)
template <class = int>  // FIXME
[[noreturn]] __attribute__((always_inline)) CONSTEXPR17 void unreachable() {
  __builtin_unreachable();
}
#elif defined(_MSC_VER)  // MSVC
[[noreturn]] __forceinline CONSTEXPR17 void unreachable() {
  __assume(false);
}
#else
  #warning "Unknown compiler. Cannot implement `unreachable()` correctly."
[[noreturn]] void unreachable() {
  std::exit(1);
}
#endif

template <bool Tagged>
struct tagged_dispatcher_caller<index_sequence_list_iterator_end, Tagged> {
  template <class Ret, class Fn, class... Storages>
  [[noreturn]] static CONSTEXPR17 Ret call(Fn&&, Storages&&...) {
    unreachable();
  }
};

template <int strategy>
struct visit_strategy {
  template <bool Tagged, class Ret, class Fn, class IndexSequenceList,
            class... Storages>
  struct dispatcher_table;

  template <bool Tagged, class Ret, class Fn, class... IndexSequences,
            class... Storages>
  struct dispatcher_table<Tagged, Ret, Fn,
                          index_sequence_list<IndexSequences...>, Storages...> {
    using dispatcher = Ret (*)(Fn&&, Storages&&...);
    static constexpr dispatcher table[] = {
        &(tagged_dispatcher_caller<IndexSequences, Tagged>::template call<
            Ret, Fn, Storages...>) ...};
  };

  template <class Ret, class IndexSequenceList, bool Tagged, class Fn,
            class... Storages>
  static CONSTEXPR17 Ret visit(std::size_t global_index, Fn&& func,
                               Storages&&... storages) {
    constexpr auto& table = dispatcher_table<Tagged, Ret, Fn, IndexSequenceList,
                                             Storages...>::table;
    return table[global_index](std::forward<Fn>(func),
                               std::forward<Storages>(storages)...);
  }
};

#define VISIT_CASE(base, index, caller, last_iterator, next_iterator)          \
case (base) + (index):                                                         \
  return caller<typename last_iterator::type, Tagged>::template call<Ret>(     \
      std::forward<Fn>(func), std::forward<Storages>(storages)...);            \
  using next_iterator [[maybe_unused]] = typename last_iterator::next

#define VISIT_4CASES(base, caller, last_prefix, prefix)                        \
  VISIT_CASE(base, 0, caller, last_prefix##3, prefix##0);                      \
  VISIT_CASE(base, 1, caller, prefix##0, prefix##1);                           \
  VISIT_CASE(base, 2, caller, prefix##1, prefix##2);                           \
  VISIT_CASE(base, 3, caller, prefix##2, prefix##3)

#define VISIT_16CASES(base, caller, last_prefix, prefix)                       \
  VISIT_4CASES(base, caller, last_prefix##3, prefix##0);                       \
  VISIT_4CASES(base + 4, caller, prefix##0, prefix##1);                        \
  VISIT_4CASES(base + 8, caller, prefix##1, prefix##2);                        \
  VISIT_4CASES(base + 12, caller, prefix##2, prefix##3)

#define VISIT_64CASES(base, caller, last_prefix, prefix)                       \
  VISIT_16CASES(base, caller, last_prefix##3, prefix##0);                      \
  VISIT_16CASES(base + 16, caller, prefix##0, prefix##1);                      \
  VISIT_16CASES(base + 32, caller, prefix##1, prefix##2);                      \
  VISIT_16CASES(base + 48, caller, prefix##2, prefix##3)

#define VISIT_256CASES(base, caller, last_prefix, prefix)                      \
  VISIT_64CASES(base, caller, last_prefix##3, prefix##0);                      \
  VISIT_64CASES(base + 64, caller, prefix##0, prefix##1);                      \
  VISIT_64CASES(base + 128, caller, prefix##1, prefix##2);                     \
  VISIT_64CASES(base + 192, caller, prefix##2, prefix##3)

#define VISIT_BODY(suffix, macro)                                              \
  using _begin##suffix = index_sequence_list_iterator<IndexSequenceList>;      \
  switch (global_index) {                                                      \
    macro(0, tagged_dispatcher_caller, _begin, _iterator);                     \
  default:                                                                     \
    return tagged_dispatcher_caller<                                           \
        index_sequence_list_iterator_end,                                      \
        Tagged>::template call<Ret>(std::forward<Fn>(func),                    \
                                    std::forward<Storages>(storages)...);      \
  }

template <>
struct visit_strategy<1> {
  template <class Ret, class IndexSequenceList, bool Tagged, class Fn,
            class... Storages>
  static CONSTEXPR17 Ret visit(std::size_t global_index, Fn&& func,
                               Storages&&... storages) {
    VISIT_BODY(3, VISIT_4CASES)
  }
};

template <>
struct visit_strategy<2> {
  template <class Ret, class IndexSequenceList, bool Tagged, class Fn,
            class... Storages>
  static CONSTEXPR17 Ret visit(std::size_t global_index, Fn&& func,
                               Storages&&... storages) {
    VISIT_BODY(33, VISIT_16CASES)
  }
};

template <>
struct visit_strategy<3> {
  template <class Ret, class IndexSequenceList, bool Tagged, class Fn,
            class... Storages>
  static CONSTEXPR17 Ret visit(std::size_t global_index, Fn&& func,
                               Storages&&... storages) {
    VISIT_BODY(333, VISIT_64CASES)
  }
};

template <>
struct visit_strategy<4> {
  template <class Ret, class IndexSequenceList, bool Tagged, class Fn,
            class... Storages>
  static CONSTEXPR17 Ret visit(std::size_t global_index, Fn&& func,
                               Storages&&... storages) {
    VISIT_BODY(3333, VISIT_256CASES)
  }
};

#undef VISIT_BODY
#undef VISIT_256CASES
#undef VISIT_64CASES
#undef VISIT_16CASES
#undef VISIT_4CASES

template <class Fn, class... Storages>
struct tagged_apply_result
    : std::invoke_result<Fn&&, decltype(get_variant_tagged_content<0>(
                                   std::declval<Storages>()))...> { };

template <std::size_t TotalSize>
struct visit_strategy_selector
    : std::conditional<TotalSize <= 4, visit_strategy<1>,
                       typename std::conditional<
                           TotalSize <= 16, visit_strategy<2>,
                           typename std::conditional<
                               TotalSize <= 64, visit_strategy<3>,
                               typename std::conditional<
                                   TotalSize <= 256, visit_strategy<4>,
                                   visit_strategy<-1>>::type>::type>::type> { };

template <class Ret, bool Tagged, class Fn, class... Storages>
CONSTEXPR17 Ret visit_impl(Fn&& func, Storages&&... storages) {
  constexpr std::size_t total_status_count =
      (std::size_t(1) * ... *
       std::remove_reference<Storages>::type::status_count);
  static_assert(total_status_count != 0, "total_status_count cannot be 0.");

  using strategy = typename visit_strategy_selector<total_status_count>::type;

  std::size_t global_index = 0;
  std::size_t _unused[] = {
      0, /* For empty `storages`. */
      (global_index =
           global_index * std::remove_reference<Storages>::type::status_count +
           storages.index())...};
  (void) _unused;

  return strategy::template visit<
      Ret,
      typename status_index_cartesian_product<
          std::remove_reference<Storages>::type::status_count...>::type,
      Tagged>(global_index, std::forward<Fn>(func),
              std::forward<Storages>(storages)...);
}

template <class Fn, class... Storages>
CONSTEXPR17 typename tagged_apply_result<Fn, Storages...>::type
tagged_visit(Fn&& func, Storages&&... storages) {
  using ret_type = typename tagged_apply_result<Fn, Storages...>::type;
  return visit_impl<ret_type, true>(std::forward<Fn>(func),
                                    std::forward<Storages>(storages)...);
}

template <class... Tys>
template <class Tagged>
CONSTEXPR17 void variant_storage<Tys...>::clear(Tagged) noexcept {
  clear<Tagged::index>();
}

template <class... Tys>
template <std::size_t Idx>
CONSTEXPR17 void variant_storage<Tys...>::clear() noexcept {
  if constexpr (!is_valueless<Idx, variant_storage>::value) {
    destroy_union_alt<Idx>();
  }
}

template <class... Tys>
CONSTEXPR17 void variant_storage<Tys...>::clear() noexcept {
  if constexpr (!std::conjunction<
                    std::is_trivially_destructible<Tys>...>::value) {
    tagged_visit([this](auto t) { this->clear(t); }, *this);
  }
}

template <class... Tys>
template <class Tagged>
CONSTEXPR17 void variant_storage<Tys...>::clear_to_valueless(Tagged) noexcept {
  static_assert(
      !variant_storage::no_valueless_state,
      "variant_storage cannot be valueless, please consider using clear().");
  clear_to_valueless<Tagged::index>();
}

template <class... Tys>
template <std::size_t Idx>
CONSTEXPR17 void variant_storage<Tys...>::clear_to_valueless() noexcept {
  static_assert(
      !variant_storage::no_valueless_state,
      "variant_storage cannot be valueless, please consider using clear().");
  clear<Idx>();
  set_raw_index(valueless_raw_index);
}

template <class... Tys>
CONSTEXPR17 void variant_storage<Tys...>::clear_to_valueless() noexcept {
  static_assert(
      !variant_storage::no_valueless_state,
      "variant_storage cannot be valueless, please consider using clear().");
  if constexpr (!std::conjunction<
                    std::is_trivially_destructible<Tys>...>::value) {
    tagged_visit([this](auto t) { this->clear(t); }, *this);
  }
  set_raw_index(valueless_raw_index);
}

/**
 * Checks if the given variant_storage supports the FALL_BACK policy.
 * Currently this requires that its first member must be
 * nothrow default-constructible. This can later be changed to fall back to
 * the first nothrow default-constructible member.
 */
template <class Storage>
struct can_fall_back : std::false_type { };

template <class Head, class... Tails>
struct can_fall_back<variant_storage<Head, Tails...>>
    : std::is_nothrow_default_constructible<Head> { };

template <class Storage>
struct variant_fall_back_guard {
  Storage* target;

  CONSTEXPR20 ~variant_fall_back_guard() {
    if (target) {
      target->template construct_union_alt<0>();
    }
  }
};

template <class... Tys>
template <std::size_t CurIdx, std::size_t TargetIdx, class... Args>
CONSTEXPR20 void variant_storage<Tys...>::emplace_alt(Args&&... args) {
  using target_type = variant_alternative_t<TargetIdx, variant<Tys...>>;

  if constexpr (std::is_nothrow_constructible<target_type, Args&&...>::value) {
    this->template clear<CurIdx>();
    this->template construct_union_alt<TargetIdx>(std::forward<Args>(args)...);
  } else if constexpr (std::is_nothrow_move_constructible<target_type>::value) {
    target_type _temp(std::forward<Args>(args)...);
    this->template clear<CurIdx>();
    this->template construct_union_alt<TargetIdx>(std::move(_temp));
  } else {
    if constexpr (variant_valueless_strategy<variant<Tys...>>::value ==
                  variant_valueless_strategy_t::FALLBACK) {
      static_assert(can_fall_back<variant_storage<Tys...>>::value,
                    "When using the FALL_BACK strategy, the first member of "
                    "the variant must be default-constructible.");

      this->template clear<CurIdx>();
      variant_fall_back_guard<
          typename std::remove_reference<decltype(this->storage())>::type>
          _guard {&this->storage()};
      this->template construct_union_alt<TargetIdx>(
          std::forward<Args>(args)...);
      _guard.target = nullptr;
    } else {
      if constexpr (variant_valueless_strategy<variant<Tys...>>::value ==
                    variant_valueless_strategy_t::LET_VARIANT_DECIDE) {
        this->template clear_to_valueless<CurIdx>();
      } else {
        this->template clear<CurIdx>();
      }
      this->template construct_union_alt<TargetIdx>(
          std::forward<Args>(args)...);
    }
  }
}

template <class... Tys>
struct variant_non_trivially_destructible_storage : variant_storage<Tys...> {
  using self = variant_non_trivially_destructible_storage;
  using base = variant_storage<Tys...>;
  using base::base;

  variant_non_trivially_destructible_storage() = default;
  variant_non_trivially_destructible_storage(const self&) = default;
  variant_non_trivially_destructible_storage(self&&) = default;
  self& operator=(const self&) = default;
  self& operator=(self&&) = default;

  CONSTEXPR20 ~variant_non_trivially_destructible_storage() { this->clear(); }
};

template <class... Tys>
using variant_destructible_storage_t = typename std::conditional<
    std::conjunction<std::is_trivially_destructible<Tys>...>::value,
    variant_storage<Tys...>,
    variant_non_trivially_destructible_storage<Tys...>>::type;

template <class... Tys>
struct variant_construct_visitor {
  variant_storage<Tys...>& self;

  template <std::size_t Idx, class Ty>
  CONSTEXPR17 void operator()(tagged_reference<Idx, Ty> source) const {
    if constexpr (!std::is_same<Ty, valueless_tag>::value) {
      self.template construct_union_alt<Idx>(source.forward_content());
    } else {
      self.set_index(Idx);
    }
  }
};

template <class... Tys>
struct variant_non_trivially_copy_constructible_storage
    : variant_destructible_storage_t<Tys...> {
  using self = variant_non_trivially_copy_constructible_storage;
  using base = variant_destructible_storage_t<Tys...>;
  using base::base;

  variant_non_trivially_copy_constructible_storage() = default;
  variant_non_trivially_copy_constructible_storage(self&&) = default;
  self& operator=(const self&) = default;
  self& operator=(self&&) = default;

  CONSTEXPR17
  variant_non_trivially_copy_constructible_storage(const self& other) noexcept(
      std::conjunction<std::is_nothrow_copy_constructible<Tys>...>::value) :
      base() {
    tagged_visit(variant_construct_visitor<Tys...> {*this}, other.storage());
  }
};

template <class... Tys>
struct variant_deleted_copy_storage : variant_destructible_storage_t<Tys...> {
  using self = variant_deleted_copy_storage;
  using base = variant_destructible_storage_t<Tys...>;
  using base::base;

  variant_deleted_copy_storage() = default;
  variant_deleted_copy_storage(const self&) = delete;
  variant_deleted_copy_storage(self&&) = default;
  self& operator=(const self&) = default;
  self& operator=(self&&) = default;
};

template <class... Tys>
using variant_copy_constructible_storage_t = typename std::conditional<
    std::conjunction<std::is_trivially_copy_constructible<Tys>...>::value,
    variant_destructible_storage_t<Tys...>,
    typename std::conditional<
        std::conjunction<std::is_copy_constructible<Tys>...>::value,
        variant_non_trivially_copy_constructible_storage<Tys...>,
        variant_deleted_copy_storage<Tys...>>::type>::type;

template <class... Tys>
struct variant_non_trivially_move_constructible_storage
    : variant_copy_constructible_storage_t<Tys...> {
  using self = variant_non_trivially_move_constructible_storage;
  using base = variant_copy_constructible_storage_t<Tys...>;
  using base::base;

  variant_non_trivially_move_constructible_storage() = default;
  variant_non_trivially_move_constructible_storage(const self&) = default;
  self& operator=(const self&) = default;
  self& operator=(self&&) = default;

  CONSTEXPR17
  variant_non_trivially_move_constructible_storage(self&& other) noexcept(
      std::conjunction<std::is_nothrow_move_constructible<Tys>...>::value) {
    tagged_visit(variant_construct_visitor<Tys...> {*this},
                 std::move(other).storage());
  }
};

template <class... Tys>
struct variant_deleted_move_storage
    : variant_copy_constructible_storage_t<Tys...> {
  using self = variant_deleted_move_storage;
  using base = variant_copy_constructible_storage_t<Tys...>;
  using base::base;

  variant_deleted_move_storage() = default;
  variant_deleted_move_storage(const self&) = default;
  variant_deleted_move_storage(self&&) = delete;
  self& operator=(const self&) = default;
  self& operator=(self&&) = default;
};

template <class... Tys>
using variant_move_constructible_storage_t = typename std::conditional<
    std::conjunction<std::is_trivially_move_constructible<Tys>...>::value,
    variant_copy_constructible_storage_t<Tys...>,
    typename std::conditional<
        std::conjunction<std::is_move_constructible<Tys>...>::value,
        variant_non_trivially_move_constructible_storage<Tys...>,
        variant_deleted_move_storage<Tys...>>::type>::type;

template <std::size_t OtherIdx, class OtherTy, class... TargetTys>
struct variant_assign_visitor_impl {
  variant_storage<TargetTys...>& self;
  tagged_reference<OtherIdx, OtherTy> source;

  template <std::size_t SelfIdx, class SelfTy>
  CONSTEXPR17 void operator()(tagged_reference<SelfIdx, SelfTy> target) const {
    if constexpr (std::is_same<OtherTy, valueless_tag>::value) {
      self.clear_to_valueless(target);
    } else if constexpr (SelfIdx == OtherIdx) {
      target.content = source.forward_content();
    } else {
      self.template emplace_alt<SelfIdx, OtherIdx>(source.forward_content());
    }
  }
};

template <class... Tys>
struct variant_assign_visitor {
  variant_storage<Tys...>& self;

  template <std::size_t SelfIdx, class SelfTy, std::size_t OtherIdx,
            class OtherTy>
  CONSTEXPR17 void
  operator()(tagged_reference<SelfIdx, SelfTy> target,
             tagged_reference<OtherIdx, OtherTy> source) const {
    return variant_assign_visitor_impl<OtherIdx, OtherTy, Tys...> {
        self, source}(target);
  }
};

template <class... Tys>
struct variant_non_trivially_copy_assignable_storage
    : variant_move_constructible_storage_t<Tys...> {
  using self = variant_non_trivially_copy_assignable_storage;
  using base = variant_move_constructible_storage_t<Tys...>;
  using base::base;

  variant_non_trivially_copy_assignable_storage() = default;
  variant_non_trivially_copy_assignable_storage(const self&) = default;
  variant_non_trivially_copy_assignable_storage(self&&) = default;
  self& operator=(self&&) = default;

  self& operator=(const self& other) noexcept(
      std::conjunction<std::is_nothrow_copy_constructible<Tys>...,
                       std::is_nothrow_copy_assignable<Tys>...>::value) {
    tagged_visit(variant_assign_visitor<Tys...> {*this}, this->storage(),
                 other.storage());
    return *this;
  }
};

template <class... Tys>
struct variant_deleted_copy_assignment_storage
    : variant_move_constructible_storage_t<Tys...> {
  using self = variant_deleted_copy_assignment_storage;
  using base = variant_move_constructible_storage_t<Tys...>;
  using base::base;

  variant_deleted_copy_assignment_storage() = default;
  variant_deleted_copy_assignment_storage(const self&) = default;
  variant_deleted_copy_assignment_storage(self&&) = default;
  self& operator=(const self&) = delete;
  self& operator=(self&&) = default;
};

template <class... Tys>
using variant_copy_assignment_storage_t = typename std::conditional<
    std::conjunction<std::is_trivially_copy_constructible<Tys>...,
                     std::is_trivially_copy_assignable<Tys>...,
                     std::is_trivially_destructible<Tys>...>::value,
    variant_move_constructible_storage_t<Tys...>,
    typename std::conditional<
        std::conjunction<std::is_copy_constructible<Tys>...,
                         std::is_copy_assignable<Tys>...>::value,
        variant_non_trivially_copy_assignable_storage<Tys...>,
        variant_deleted_copy_assignment_storage<Tys...>>::type>::type;

template <class... Tys>
struct variant_non_trivially_move_assignable_storage
    : variant_copy_assignment_storage_t<Tys...> {
  using self = variant_non_trivially_move_assignable_storage;
  using base = variant_copy_assignment_storage_t<Tys...>;
  using base::base;

  variant_non_trivially_move_assignable_storage() = default;
  variant_non_trivially_move_assignable_storage(const self&) = default;
  variant_non_trivially_move_assignable_storage(self&&) = default;
  self& operator=(const self&) = default;

  self& operator=(self&& other) noexcept(
      std::conjunction<std::is_nothrow_move_constructible<Tys>...,
                       std::is_nothrow_move_assignable<Tys>...>::value) {
    tagged_visit(variant_assign_visitor<Tys...> {*this}, this->storage(),
                 std::move(other).storage());
    return *this;
  }
};

template <class... Tys>
struct variant_deleted_move_assignment_storage
    : variant_copy_assignment_storage_t<Tys...> {
  using self = variant_deleted_move_assignment_storage;
  using base = variant_copy_assignment_storage_t<Tys...>;
  using base::base;

  variant_deleted_move_assignment_storage() = default;
  variant_deleted_move_assignment_storage(const self&) = default;
  variant_deleted_move_assignment_storage(self&&) = default;
  self& operator=(const self&) = default;
  self& operator=(self&&) = delete;
};

template <class... Tys>
using variant_move_assignment_storage_t = typename std::conditional<
    std::conjunction<std::is_trivially_move_constructible<Tys>...,
                     std::is_trivially_move_assignable<Tys>...,
                     std::is_trivially_destructible<Tys>...>::value,
    variant_copy_assignment_storage_t<Tys...>,
    typename std::conditional<
        std::conjunction<std::is_move_constructible<Tys>...,
                         std::is_move_assignable<Tys>...>::value,
        variant_non_trivially_move_assignable_storage<Tys...>,
        variant_deleted_move_assignment_storage<Tys...>>::type>::type;

template <class Ti>
void narrow_conversion_checker(Ti (&&)[1]);

template <class Ty, std::size_t Idx>
struct variant_conversion_constructor_selected_alternative {
  using type = Ty;
  static constexpr std::size_t index = Idx;
};

template <std::size_t Idx, class Ti, class Ty, class = void>
struct check_narrow_conversion { };

// An overload F(T_i) is only considered if the declaration T_i x[] = {
// std::forward<T>(t) }; is valid for some invented variable x.
template <std::size_t Idx, class Ti, class Ty>
struct check_narrow_conversion<
    Idx, Ti, Ty,
    std::void_t<decltype(narrow_conversion_checker<Ti>(
        {std::declval<Ty>()}))>> {
  using type = variant_conversion_constructor_selected_alternative<Ti, Idx>;
};

template <std::size_t Idx, class Ti>
struct imaginary_function_wrapper {
  template <class Ty>
  static typename check_narrow_conversion<Idx, Ti, Ty>::type
      imaginary_function(Ti);
};

template <class, class...>
struct imaginary_function_overload_impl;

template <std::size_t... Is, class... Tys>
struct imaginary_function_overload_impl<std::index_sequence<Is...>, Tys...>
    : imaginary_function_wrapper<Is, Tys>... {
  using imaginary_function_wrapper<Is, Tys>::imaginary_function...;
};

template <class... Tys>
using imaginary_function_overload =
    imaginary_function_overload_impl<std::index_sequence_for<Tys...>, Tys...>;

template <class enable, class Ty, class... Tis>
struct conversion_constructor_index_selector_impl { };

template <class Ty, class... Tis>
struct conversion_constructor_index_selector_impl<
    std::void_t<
        decltype(imaginary_function_overload<
                 Tis...>::template imaginary_function<Ty>(std::declval<Ty>()))>,
    Ty, Tis...> {
  using type =
      decltype(imaginary_function_overload<Tis...>::template imaginary_function<
               Ty>(std::declval<Ty>()));
};

// Constructs a variant holding the alternative type T_j that would be selected
// by overload resolution for the expression F(std::forward<T>(t)) if there was
// an overload of imaginary function F(T_i) for every T_i from Types... in scope
// at the same time.
template <class Ty, class... Tis>
using conv_ctor_index =
    typename conversion_constructor_index_selector_impl<void, Ty, Tis...>::type;

template <class Ty, class... Tys>
constexpr std::size_t find_match_index() {
  constexpr bool results[] = {std::is_same<Ty, Tys>::value...};
  std::size_t match_count = 0;
  auto result = static_cast<std::size_t>(-1);

  for (std::size_t i = 0; i != std::size(results); ++i) {
    if (results[i]) {
      result = i;
      ++match_count;
    }
  }

  if (match_count == 1)
    return result;
  return static_cast<std::size_t>(-1);
}

template <class... Tys>
class variant : private variant_move_assignment_storage_t<Tys...> {
  static_assert(
      std::conjunction<std::is_destructible<Tys>...>::value,
      "Cpp17Destructible requires that all types must be destructible.");
  static_assert(std::conjunction<std::is_object<Tys>...,
                                 std::negation<std::is_array<Tys>>...>::value,
                "Array types and non-object types are not Cpp17Destructible.");
  static_assert(sizeof...(Tys) > 0, "Cannot construct empty variant.");

  using base = variant_move_assignment_storage_t<Tys...>;

  template <class Ty>
  struct is_specialization_of_in_place : std::false_type { };

  template <class Ty>
  struct is_specialization_of_in_place<std::in_place_type_t<Ty>>
      : std::true_type { };

  template <std::size_t Idx>
  struct is_specialization_of_in_place<std::in_place_index_t<Idx>>
      : std::true_type { };
public:
  using base::storage;

  template <class FirstType = variant_alternative_t<0, variant>,
            typename std::enable_if<
                std::is_default_constructible<FirstType>::value, int>::type = 0>
  CONSTEXPR17
  variant() noexcept(std::is_nothrow_default_constructible<FirstType>::value) :
      base(std::in_place_index<0>) { }

  CONSTEXPR17 variant(const variant&) = default;
  CONSTEXPR17 variant(variant&&) = default;

  template <class Ty,
            typename std::enable_if<
                std::conjunction<
                    std::bool_constant<(sizeof...(Tys) > 0)>,
                    std::negation<
                        std::is_same<typename remove_cvref<Ty>::type, variant>>,
                    std::negation<is_specialization_of_in_place<
                        typename remove_cvref<Ty>::type>>,
                    std::is_constructible<
                        typename conv_ctor_index<Ty, Tys...>::type, Ty>>::value,
                int>::type = 0>
  CONSTEXPR17 variant(Ty&& other) noexcept(  //
      std::is_nothrow_constructible<typename conv_ctor_index<Ty, Tys...>::type,
                                    Ty>::value) :
      base(std::in_place_index<conv_ctor_index<Ty, Tys...>::index>,
           std::forward<Ty>(other)) { }

  template <
      class Ty, class... Args,
      std::size_t MatchIndex = find_match_index<Ty, Tys...>(),
      typename std::enable_if<MatchIndex != static_cast<std::size_t>(-1) &&
                                  std::is_constructible<Ty, Args...>::value,
                              int>::type = 0>
  CONSTEXPR17 explicit variant(std::in_place_type_t<Ty>,
                               Args&&... args) noexcept(  //
      std::is_nothrow_constructible<Ty, Args...>::value) :
      base(std::in_place_index<MatchIndex>, std::forward<Args>(args)...) { }

  template <class Ty, class U, class... Args,
            std::size_t MatchIndex = find_match_index<Ty, Tys...>(),
            typename std::enable_if<
                MatchIndex != static_cast<std::size_t>(-1) &&
                    std::is_constructible<Ty, std::initializer_list<U>&,
                                          Args...>::value,
                int>::type = 0>
  CONSTEXPR17 explicit variant(std::in_place_type_t<Ty>,
                               std::initializer_list<U> list,
                               Args&&... args) noexcept(  //
      std::is_nothrow_constructible<Ty, std::initializer_list<U>&,
                                    Args...>::value) :
      base(std::in_place_index<MatchIndex>, list, std::forward<Args>(args)...) {
  }

  template <
      std::size_t Idx, class... Args,
      typename std::enable_if<(Idx < sizeof...(Tys)), int>::type = 0,
      class TargetType = variant_alternative_t<Idx, variant>,
      typename std::enable_if<std::is_constructible<TargetType, Args...>::value,
                              int>::type = 0>
  CONSTEXPR17 explicit variant(std::in_place_index_t<Idx>,
                               Args&&... args) noexcept(  //
      std::is_nothrow_constructible<TargetType, Args&&...>::value) :
      base(std::in_place_index<Idx>, std::forward<Args>(args)...) { }

  template <std::size_t Idx, class U, class... Args,
            typename std::enable_if<(Idx < sizeof...(Tys)), int>::type = 0,
            class TargetType = variant_alternative_t<Idx, variant>,
            typename std::enable_if<
                std::is_constructible<TargetType, std::initializer_list<U>&,
                                      Args...>::value,
                int>::type = 0>
  CONSTEXPR17 explicit variant(std::in_place_index_t<Idx>,
                               std::initializer_list<U> list,
                               Args&&... args) noexcept(  //
      std::is_nothrow_constructible<TargetType, std::initializer_list<U>&,
                                    Args&&...>::value) :
      base(std::in_place_index<Idx>, list, std::forward<Args>(args)...) { }

  variant& operator=(const variant&) = default;
  variant& operator=(variant&&) = default;

  template <class Ty, class Selected = conv_ctor_index<Ty, Tys...>,
            typename std::enable_if<
                std::conjunction<
                    std::negation<
                        std::is_same<typename remove_cvref<Ty>::type, variant>>,
                    std::is_assignable<typename Selected::type&, Ty>,
                    std::is_constructible<typename Selected::type, Ty>>::value,
                int>::type = 0>
  CONSTEXPR17 variant& operator=(Ty&& other) noexcept(  //
      (std::is_nothrow_assignable<typename Selected::type&, Ty>::value) &&
      std::is_nothrow_constructible<typename Selected::type, Ty>::value) {
    tagged_visit(
        variant_assign_visitor_impl<Selected::index, Ty&&, Tys...> {
            *this,
            tagged_reference<Selected::index, Ty&&> {std::forward<Ty>(other)}},
        this->storage());
    return *this;
  }

  using base::index;
  using base::valueless_by_exception;

private:
  template <std::size_t Idx, class... Args>
  CONSTEXPR20 variant_alternative_t<Idx, variant>&
  emplace_impl(Args&&... args) {
    tagged_visit(
        [&](auto t) {
          this->template emplace_alt<decltype(t)::index, Idx>(
              std::forward<Args>(args)...);
        },
        this->storage());
    return get_variant_tagged_content<Idx>(this->storage()).content;
  }

public:
  template <
      class Ty, class... Args,
      std::size_t MatchIndex = find_match_index<Ty, Tys...>(),
      typename std::enable_if<MatchIndex != static_cast<std::size_t>(-1) &&
                                  std::is_constructible<Ty, Args...>::value,
                              int>::type = 0>
  CONSTEXPR20 Ty& emplace(Args&&... args) noexcept(
      std::is_nothrow_constructible<Ty, Args...>::value) {
    return emplace_impl<MatchIndex>(std::forward<Args>(args)...);
  }

  template <class Ty, class U, class... Args,
            std::size_t MatchIndex = find_match_index<Ty, Tys...>(),
            typename std::enable_if<
                MatchIndex != static_cast<std::size_t>(-1) &&
                    std::is_constructible<Ty, std::initializer_list<U>&,
                                          Args...>::value,
                int>::type = 0>
  CONSTEXPR17 Ty&
  emplace(std::initializer_list<U> list, Args&&... args) noexcept(
      std::is_nothrow_constructible<Ty, std::initializer_list<U>&,
                                    Args...>::value) {
    return emplace_impl<MatchIndex>(list, std::forward<Args>(args)...);
  }

  template <
      std::size_t Idx, class... Args,
      class TargetType = variant_alternative_t<Idx, variant>,
      typename std::enable_if<std::is_constructible<TargetType, Args...>::value,
                              int>::type = 0>
  CONSTEXPR17 TargetType& emplace(Args&&... args) noexcept(
      std::is_nothrow_constructible<TargetType, Args...>::value) {
    return emplace_impl<Idx>(std::forward<Args>(args)...);
  }

  template <std::size_t Idx, class U, class... Args,
            class TargetType = variant_alternative_t<Idx, variant>,
            typename std::enable_if<
                std::is_constructible<TargetType, std::initializer_list<U>&,
                                      Args...>::value,
                int>::type = 0>
  CONSTEXPR17 TargetType&
  emplace(std::initializer_list<U> list, Args&&... args) noexcept(
      std::is_nothrow_constructible<TargetType, std::initializer_list<U>&,
                                    Args...>::value) {
    return emplace_impl<Idx>(list, std::forward<Args>(args)...);
  }

private:
  struct _swap_impl {
    variant& lhs_variant;
    variant& rhs_variant;

    template <std::size_t LhsIdx, class LhsType, std::size_t RhsIdx,
              class RhsType>
    void operator()(tagged_reference<LhsIdx, LhsType> lhs,
                    tagged_reference<RhsIdx, RhsType> rhs) const {
      if constexpr (std::is_same<RhsType, valueless_tag>::value) {
        unreachable();
      } else if constexpr (LhsIdx == RhsIdx) {
        using std::swap;
        swap(lhs.content, rhs.content);
      } else {
        if constexpr (std::is_same<LhsType, valueless_tag>::value) {
          lhs_variant.template emplace_alt<LhsIdx, RhsIdx>(
              std::move(rhs.content));
          rhs_variant.clear_to_valueless(rhs);
        } else {
          auto _temp(std::move(rhs.content));
          rhs_variant.template emplace_alt<RhsIdx, LhsIdx>(
              std::move(lhs.content));
          lhs_variant.template emplace_alt<LhsIdx, RhsIdx>(std::move(_temp));
        }
      }
    }
  };

public:
  CONSTEXPR20 void swap(variant& rhs) noexcept(
      std::conjunction<std::is_nothrow_move_constructible<Tys>...,
                       std::is_nothrow_swappable<Tys>...>::value) {

    if (this->valueless_by_exception() && rhs.valueless_by_exception()) {
      return;
    } else {
      auto _lhs_ptr = this, _rhs_ptr = &rhs;
      if (rhs.valueless_by_exception())
        std::swap(_lhs_ptr, _rhs_ptr);

      tagged_visit(_swap_impl {*_lhs_ptr, *_rhs_ptr}, _lhs_ptr->storage(),
                   _rhs_ptr->storage());
    }
  }
};

// The implementation of as-variant.
template <class... Tys>
CONSTEXPR17 variant<Tys...>& as_variant(variant<Tys...>& var) {
  return var;
}

template <class... Tys>
CONSTEXPR17 const variant<Tys...>& as_variant(const variant<Tys...>& var) {
  return var;
}

template <class... Tys>
CONSTEXPR17 variant<Tys...>&& as_variant(variant<Tys...>&& var) {
  return std::move(var);
}

template <class... Tys>
CONSTEXPR17 const variant<Tys...>&& as_variant(const variant<Tys...>&& var) {
  return std::move(var);
}

template <class Fn, class... Variants>
struct apply_result
    : std::invoke_result<Fn&&, decltype(get_variant_tagged_content<0>(
                                            std::declval<Variants>().storage())
                                            .forward_content())...> { };

template <class Fn, template <class...> class check_trait, class Check,
          class IndexSequenceList, class... Storages>
struct visit_return_type_checker;

template <class Fn, class IndexSequence, class... Storages>
struct single_visit_return_type;

template <class Fn, std::size_t... Is, class... Storages>
struct single_visit_return_type<Fn, std::index_sequence<Is...>, Storages...>
    : std::invoke_result<
          Fn&&, decltype(get_variant_tagged_content<(
                             std::remove_reference<
                                     Storages>::type::valueless_raw_index == Is
                                 ? 0
                                 : Is)>(std::declval<Storages>())
                             .forward_content())...> { };

template <class Fn, template <class...> class check_trait, class CheckBase,
          class... IndexSequences, class... Storages>
struct visit_return_type_checker<Fn, check_trait, CheckBase,
                                 index_sequence_list<IndexSequences...>,
                                 Storages...>
    : std::conjunction<check_trait<
          CheckBase, typename single_visit_return_type<Fn, IndexSequences,
                                                       Storages...>::type>...> {
};

template <
    class Visitor, class... Variants,
    class = std::void_t<decltype(as_variant(std::declval<Variants>()))...>>
CONSTEXPR17 typename apply_result<
    Visitor, decltype(as_variant(std::declval<Variants>()))...>::type
visit(Visitor&& visitor, Variants&&... variants) {
  using ret_type = typename apply_result<Visitor, Variants...>::type;
  // FIXME: Try moving this check into visit_impl.
  using index_sequences =
      typename status_index_cartesian_product<std::remove_reference<
          decltype(variants.storage())>::type::status_count...>::type;
  static_assert(
      visit_return_type_checker<
          Visitor, std::is_same, ret_type, index_sequences,
          decltype(std::forward<Variants>(variants).storage())...>::value,
      "The call is ill-formed if the invocation is not a valid expression of "
      "the same type and value category, for all combinations of "
      "alternative types of all variants.");

  return visit_impl<ret_type, false>(
      std::forward<Visitor>(visitor),
      as_variant(std::forward<Variants>(variants)).storage()...);
}

template <
    class Ret, class Visitor, class... Variants,
    class = std::void_t<decltype(as_variant(std::declval<Variants>()))...>>
CONSTEXPR17 Ret visit(Visitor&& visitor, Variants&&... variants) {
  // TODO: Add some checks here.
  return visit_impl<Ret, false>(
      std::forward<Visitor>(visitor),
      as_variant(std::forward<Variants>(variants)).storage()...);
}

template <class Ty, class... Tys>
CONSTEXPR17 bool holds_alternative(const variant<Tys...>& var) noexcept {
  constexpr std::size_t match_index = find_match_index<Ty, Tys...>();
  static_assert(match_index != static_cast<std::size_t>(-1),
                "The type Ty must occur exactly once in Tys.");

  return var.index() == match_index;
}

template <std::size_t Idx, class Variant>
CONSTEXPR17 decltype(get_variant_tagged_content<Idx>(
                         std::declval<Variant>().storage())
                         .forward_content())
get_impl(Variant&& var) {
  if (var.index() != Idx)
    throw_bad_variant_access();
  return get_variant_tagged_content<Idx>(std::forward<Variant>(var).storage())
      .forward_content();
}

template <std::size_t Idx, class... Tys>
CONSTEXPR17 variant_alternative_t<Idx, variant<Tys...>>&
get(variant<Tys...>& v) {
  static_assert(Idx < sizeof...(Tys), "index Idx out of bound");
  return get_impl<Idx>(v);
}

template <std::size_t Idx, class... Tys>
CONSTEXPR17 variant_alternative_t<Idx, variant<Tys...>>&&
get(variant<Tys...>&& v) {
  static_assert(Idx < sizeof...(Tys), "index Idx out of bound");
  return get_impl<Idx>(std::move(v));
}

template <std::size_t Idx, class... Tys>
CONSTEXPR17 const variant_alternative_t<Idx, variant<Tys...>>&
get(const variant<Tys...>& v) {
  static_assert(Idx < sizeof...(Tys), "index Idx out of bound");
  return get_impl<Idx>(v);
}

template <std::size_t Idx, class... Tys>
CONSTEXPR17 const variant_alternative_t<Idx, variant<Tys...>>&&
get(const variant<Tys...>&& v) {
  static_assert(Idx < sizeof...(Tys), "index Idx out of bound");
  return get_impl<Idx>(std::move(v));
}

template <class Ty, class... Tys>
CONSTEXPR17 Ty& get(variant<Tys...>& v) {
  constexpr std::size_t match_index = find_match_index<Ty, Tys...>();
  static_assert(match_index != static_cast<std::size_t>(-1),
                "The type Ty must occur exactly once in Tys.");

  return get_impl<match_index>(v);
}

template <class Ty, class... Tys>
CONSTEXPR17 Ty&& get(variant<Tys...>&& v) {
  constexpr std::size_t match_index = find_match_index<Ty, Tys...>();
  static_assert(match_index != static_cast<std::size_t>(-1),
                "The type Ty must occur exactly once in Tys.");

  return get_impl<match_index>(std::move(v));
}

template <class Ty, class... Tys>
CONSTEXPR17 const Ty& get(const variant<Tys...>& v) {
  constexpr std::size_t match_index = find_match_index<Ty, Tys...>();
  static_assert(match_index != static_cast<std::size_t>(-1),
                "The type Ty must occur exactly once in Tys.");

  return get_impl<match_index>(v);
}

template <class Ty, class... Tys>
CONSTEXPR17 const Ty&& get(const variant<Tys...>&& v) {
  constexpr std::size_t match_index = find_match_index<Ty, Tys...>();
  static_assert(match_index != static_cast<std::size_t>(-1),
                "The type Ty must occur exactly once in Tys.");

  return get_impl<match_index>(std::move(v));
}

template <std::size_t Idx, class Variant>
CONSTEXPR17 typename std::add_pointer<
    decltype(get_variant_tagged_content<Idx>(std::declval<Variant>().storage())
                 .content)>::type
get_if_impl(Variant* pv) noexcept {
  return pv && pv->index() == Idx
             ? std::addressof(
                   get_variant_tagged_content<Idx>(pv->storage()).content)
             : nullptr;
}

template <std::size_t Idx, class... Tys>
CONSTEXPR17 typename std::add_pointer<  //
    variant_alternative_t<Idx, variant<Tys...>>>::type
get_if(variant<Tys...>* pv) noexcept {
  static_assert(Idx < sizeof...(Tys), "index Idx out of bound");
  return get_if_impl<Idx>(pv);
}

template <std::size_t Idx, class... Tys>
CONSTEXPR17 typename std::add_pointer<
    const variant_alternative_t<Idx, variant<Tys...>>>::type
get_if(const variant<Tys...>* pv) noexcept {
  static_assert(Idx < sizeof...(Tys), "index Idx out of bound");
  return get_if_impl<Idx>(pv);
}

template <class Ty, class... Tys>
CONSTEXPR17 typename std::add_pointer<Ty>::type
get_if(variant<Tys...>* pv) noexcept {
  constexpr std::size_t match_index = find_match_index<Ty, Tys...>();
  static_assert(match_index != static_cast<std::size_t>(-1),
                "The type Ty must occur exactly once in Tys.");

  return get_if_impl<match_index>(pv);
}

template <class Ty, class... Tys>
CONSTEXPR17 typename std::add_pointer<const Ty>::type
get_if(const variant<Tys...>* pv) noexcept {
  constexpr std::size_t match_index = find_match_index<Ty, Tys...>();
  static_assert(match_index != static_cast<std::size_t>(-1),
                "The type Ty must occur exactly once in Tys.");

  return get_if_impl<match_index>(pv);
}

template <class Op, class Variant>
struct variant_comparison_impl {
  const Variant& lhs;

  template <std::size_t Idx, class Ty>
  CONSTEXPR17 bool operator()(tagged_reference<Idx, Ty> rhs) const {
    if constexpr (std::is_same<Ty, valueless_tag>::value) {
      unreachable();
    } else {
      return Op {}(
          get_variant_tagged_content<Idx>(lhs.storage()).forward_content(),
          rhs.forward_content());
    }
  }
};

// TODO: noexcept
template <class... Tys>
CONSTEXPR17 bool
operator==(const variant<Tys...>& lhs, const variant<Tys...>& rhs) noexcept(
    std::conjunction<std::is_nothrow_invocable<std::equal_to<>, const Tys&,
                                               const Tys&>...>::value) {
  return lhs.index() == rhs.index() &&
         (lhs.valueless_by_exception() ||
          tagged_visit(
              variant_comparison_impl<std::equal_to<>, variant<Tys...>> {lhs},
              rhs.storage()));
}

template <class... Tys>
CONSTEXPR17 bool
operator!=(const variant<Tys...>& lhs, const variant<Tys...>& rhs) noexcept(
    std::conjunction<std::is_nothrow_invocable<std::not_equal_to<>, const Tys&,
                                               const Tys&>...>::value) {
  return lhs.index() != rhs.index() ||
         (!lhs.valueless_by_exception() &&
          tagged_visit(
              variant_comparison_impl<std::not_equal_to<>, variant<Tys...>> {
                  lhs},
              rhs.storage()));
}

template <class... Tys>
CONSTEXPR17 bool
operator<(const variant<Tys...>& lhs, const variant<Tys...>& rhs) noexcept(
    std::conjunction<std::is_nothrow_invocable<std::less<>, const Tys&,
                                               const Tys&>...>::value) {
  if (rhs.valueless_by_exception())
    return false;
  if (lhs.valueless_by_exception())
    return true;
  return lhs.index() < rhs.index() ||
         (lhs.index() == rhs.index() &&
          tagged_visit(
              variant_comparison_impl<std::less<>, variant<Tys...>> {lhs},
              rhs.storage()));
}

template <class... Tys>
CONSTEXPR17 bool
operator>(const variant<Tys...>& lhs, const variant<Tys...>& rhs) noexcept(
    std::conjunction<std::is_nothrow_invocable<std::greater<>, const Tys&,
                                               const Tys&>...>::value) {
  if (lhs.valueless_by_exception())
    return false;
  if (rhs.valueless_by_exception())
    return true;
  return lhs.index() > rhs.index() ||
         (lhs.index() == rhs.index() &&
          tagged_visit(
              variant_comparison_impl<std::greater<>, variant<Tys...>> {lhs},
              rhs.storage()));
}

template <class... Tys>
CONSTEXPR17 bool
operator<=(const variant<Tys...>& lhs, const variant<Tys...>& rhs) noexcept(
    std::conjunction<std::is_nothrow_invocable<std::less_equal<>, const Tys&,
                                               const Tys&>...>::value) {
  if (lhs.valueless_by_exception())
    return true;
  if (rhs.valueless_by_exception())
    return false;
  return lhs.index() < rhs.index() ||
         (lhs.index() == rhs.index() &&
          tagged_visit(
              variant_comparison_impl<std::less_equal<>, variant<Tys...>> {lhs},
              rhs.storage()));
}

template <class... Tys>
CONSTEXPR17 bool
operator>=(const variant<Tys...>& lhs, const variant<Tys...>& rhs) noexcept(
    std::conjunction<std::is_nothrow_invocable<std::greater_equal<>, const Tys&,
                                               const Tys&>...>::value) {
  if (rhs.valueless_by_exception())
    return true;
  if (lhs.valueless_by_exception())
    return false;
  return lhs.index() > rhs.index() ||
         (lhs.index() == rhs.index() &&
          tagged_visit(
              variant_comparison_impl<std::greater_equal<>, variant<Tys...>> {
                  lhs},
              rhs.storage()));
}

template <class... Tys, typename std::enable_if<
                            std::conjunction<std::is_move_constructible<Tys>...,
                                             std::is_swappable<Tys>...>::value,
                            int>::type = 0>
CONSTEXPR20 void swap(variant<Tys...>& lhs,
                      variant<Tys...>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
  lhs.swap(rhs);
}

struct monostate { };

CONSTEXPR17 bool operator==(monostate, monostate) noexcept {
  return true;
}

CONSTEXPR17 bool operator!=(monostate, monostate) noexcept {
  return false;
}

CONSTEXPR17 bool operator<(monostate, monostate) noexcept {
  return false;
}

CONSTEXPR17 bool operator>(monostate, monostate) noexcept {
  return false;
}

CONSTEXPR17 bool operator<=(monostate, monostate) noexcept {
  return true;
}

CONSTEXPR17 bool operator>=(monostate, monostate) noexcept {
  return true;
}

}  // namespace rust

#undef std

namespace std {
template <>
struct hash<::rust::monostate> {
  // TODO: Implementation
};

template <class... Tys>
struct hash<::rust::variant<Tys...>> {
  // TODO: Implementation
};
}  // namespace std

#undef CONSTEXPR11
#undef CONSTEXPR14
#undef CONSTEXPR17
#undef CONSTEXPR20

#endif  // RUST_ENUM_VARIANT_HPP