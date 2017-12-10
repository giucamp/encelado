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

    // tl_push_back< type_list<...>, type_list<...>, types... >
    template <typename... TYPES_1, typename... TYPES_2, typename... TYPES_3>
    struct tl_push_back<type_list<TYPES_1...>, type_list<TYPES_2...>, TYPES_3...>
    {
        using type = typename tl_push_back<type_list<TYPES_1..., TYPES_2...>, TYPES_3...>::type;
    };
    template <typename... ARGS> using tl_push_back_t = typename tl_push_back<ARGS...>::type;
}