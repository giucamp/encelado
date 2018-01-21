#pragma once
#include <ediacaran/core/ediacaran_common.h>

namespace ediacaran
{
    // type_list
    template <typename... TYPES> struct type_list
    {
        static constexpr size_t size = sizeof...(TYPES);

        template <typename STREAM> static void dbg_print(STREAM & i_stream)
        {
            ((i_stream << typeid(TYPES).name() << "\n"), ...);
        }
    };

    // tl_contains<TARGET_TYPE, type_list>, tl_contains_v
    template <typename...> struct tl_contains;
    template <typename... LIST_TYPES, typename TARGET_TYPE>
    struct tl_contains<TARGET_TYPE, type_list<LIST_TYPES...>>
        : std::disjunction<std::is_same<TARGET_TYPE, LIST_TYPES>...>
    {
    };
    template <typename... LIST_TYPES, typename TARGET_TYPE>
    struct tl_contains<TARGET_TYPE, LIST_TYPES...>
        : std::disjunction<std::is_same<TARGET_TYPE, LIST_TYPES>...>
    {
    };
    template <typename... PARAMS> constexpr bool tl_contains_v = tl_contains<PARAMS...>::value;

    // tl_difference_impl<type_list<>, type_list<...>, type_list<...>> - types of the first list which are not present in the second list
    namespace detail
    {
        template <typename...> struct tl_difference_impl;
        template <
          typename... RESULT_TYPES,
          typename FIRST_TYPE_1,
          typename... OTHER_TYPES_1,
          typename... TYPES_2>
        struct tl_difference_impl<
          type_list<RESULT_TYPES...>,
          type_list<FIRST_TYPE_1, OTHER_TYPES_1...>,
          type_list<TYPES_2...>>
        {
            using type = typename std::conditional_t<
              tl_contains_v<FIRST_TYPE_1, TYPES_2...>,
              typename tl_difference_impl<
                type_list<RESULT_TYPES...>,
                type_list<OTHER_TYPES_1...>,
                type_list<TYPES_2...>>::type,
              typename tl_difference_impl<
                type_list<RESULT_TYPES..., FIRST_TYPE_1>,
                type_list<OTHER_TYPES_1...>,
                type_list<TYPES_2...>>::type>;
        };
        template <typename... RESULT_TYPES, typename... TYPES_2>
        struct tl_difference_impl<type_list<RESULT_TYPES...>, type_list<>, type_list<TYPES_2...>>
        {
            using type = type_list<RESULT_TYPES...>;
        };
    } // namespace detail
    template <typename... PARAMS>
    using tl_difference = detail::tl_difference_impl<type_list<>, PARAMS...>;
    template <typename... PARAMS>
    using tl_difference_t = typename detail::tl_difference_impl<type_list<>, PARAMS...>::type;

    // tl_push_back, tl_push_back_t
    template <typename...> struct tl_push_back;
    template <typename... TYPES> struct tl_push_back<type_list<TYPES...>>
    {
        using type = type_list<TYPES...>;
    };
    // tl_push_back< type_list<...>, type, ... >
    template <typename... TYPES_1, typename TYPE_2, typename... TYPES_3>
    struct tl_push_back<type_list<TYPES_1...>, TYPE_2, TYPES_3...>
    {
        using type = typename tl_push_back<type_list<TYPES_1..., TYPE_2>, TYPES_3...>::type;
    };

    // tl_push_back< type_list<...>, type_list<...>, ... >
    template <typename... TYPES_1, typename... TYPES_2, typename... TYPES_3>
    struct tl_push_back<type_list<TYPES_1...>, type_list<TYPES_2...>, TYPES_3...>
    {
        using type = typename tl_push_back<type_list<TYPES_1..., TYPES_2...>, TYPES_3...>::type;
    };
    template <typename... PARAMS> using tl_push_back_t = typename tl_push_back<PARAMS...>::type;


    // tl_remove_duplicates, tl_remove_duplicates_t
    namespace detail
    {
        template <typename... TYPES> struct tl_remove_duplicates_impl;

        template <typename... RESULTS>
        struct tl_remove_duplicates_impl<type_list<RESULTS...>, type_list<>>
        {
            using type = type_list<RESULTS...>;
        };

        template <typename... RESULTS, typename CURR_TYPE, typename... NEXT_TYPES>
        struct tl_remove_duplicates_impl<type_list<RESULTS...>, type_list<CURR_TYPE, NEXT_TYPES...>>
        {
            using type = std::conditional_t<
              tl_contains_v<CURR_TYPE, RESULTS...>,
              typename tl_remove_duplicates_impl<type_list<RESULTS...>, type_list<NEXT_TYPES...>>::
                type,
              typename tl_remove_duplicates_impl<
                type_list<RESULTS..., CURR_TYPE>,
                type_list<NEXT_TYPES...>>::type>;
        };

    } // namespace detail

    template <typename... PARAMS>
    using tl_remove_duplicates_t =
      typename detail::tl_remove_duplicates_impl<type_list<>, PARAMS...>::type;
    template <typename... PARAMS>
    struct tl_remove_duplicates : detail::tl_remove_duplicates_impl<type_list<>, PARAMS...>
    {
    };

    template <typename TYPE> struct is_type_list : std::false_type
    {
    };
    template <typename... TYPES> struct is_type_list<type_list<TYPES...>> : std::true_type
    {
    };
    template <typename TYPE> constexpr bool is_type_list_v = is_type_list<TYPE>::value;
}
