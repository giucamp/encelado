
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <ediacaran/core/ediacaran_common.h>

namespace edi
{
    template <typename FUNC> struct remove_noexcept
    {
    };

    template <> struct remove_noexcept<std::nullptr_t>
    {
        using type = std::nullptr_t;
    };

    template <typename RETURN_VALUE, typename... PARAMETERS>
    struct remove_noexcept<RETURN_VALUE(PARAMETERS...)>
    {
        using type = RETURN_VALUE(PARAMETERS...);
    };

    template <typename RETURN_VALUE, typename... PARAMETERS>
    struct remove_noexcept<RETURN_VALUE(PARAMETERS...) noexcept>
    {
        using type = RETURN_VALUE(PARAMETERS...);
    };

    template <typename RETURN_VALUE, typename... PARAMETERS>
    struct remove_noexcept<RETURN_VALUE (*)(PARAMETERS...)>
    {
        using type = RETURN_VALUE (*)(PARAMETERS...);
    };

    template <typename RETURN_VALUE, typename... PARAMETERS>
    struct remove_noexcept<RETURN_VALUE (*)(PARAMETERS...) noexcept>
    {
        using type = RETURN_VALUE (*)(PARAMETERS...);
    };

#define IMPLEMENT_REMOVE_NOEXCEPT_MEM(Qualifiers)                                                  \
    template <typename CLASS, typename RETURN_VALUE, typename... PARAMETERS>                       \
    struct remove_noexcept<RETURN_VALUE (CLASS::*)(PARAMETERS...) Qualifiers>                      \
    {                                                                                              \
        using type = RETURN_VALUE (CLASS::*)(PARAMETERS...) Qualifiers;                            \
    };                                                                                             \
    template <typename CLASS, typename RETURN_VALUE, typename... PARAMETERS>                       \
    struct remove_noexcept<RETURN_VALUE (CLASS::*)(PARAMETERS...) Qualifiers noexcept>             \
    {                                                                                              \
        using type = RETURN_VALUE (CLASS::*)(PARAMETERS...) Qualifiers;                            \
    }

#define IMPLEMENT_REMOVE_NOEXCEPT_MEM_NOTHING
    IMPLEMENT_REMOVE_NOEXCEPT_MEM(IMPLEMENT_REMOVE_NOEXCEPT_MEM_NOTHING);
    IMPLEMENT_REMOVE_NOEXCEPT_MEM(const);
    IMPLEMENT_REMOVE_NOEXCEPT_MEM(volatile);
    IMPLEMENT_REMOVE_NOEXCEPT_MEM(const volatile);
    IMPLEMENT_REMOVE_NOEXCEPT_MEM(&);
    IMPLEMENT_REMOVE_NOEXCEPT_MEM(const &);
    IMPLEMENT_REMOVE_NOEXCEPT_MEM(volatile &);
    IMPLEMENT_REMOVE_NOEXCEPT_MEM(const volatile &);
    IMPLEMENT_REMOVE_NOEXCEPT_MEM(&&);
    IMPLEMENT_REMOVE_NOEXCEPT_MEM(const &&);
    IMPLEMENT_REMOVE_NOEXCEPT_MEM(volatile &&);
    IMPLEMENT_REMOVE_NOEXCEPT_MEM(const volatile &&);
#undef IMPLEMENT_REMOVE_NOEXCEPT_MEM
#undef IMPLEMENT_REMOVE_NOEXCEPT_MEM_NOTHING

    template <typename FUNC> using remove_noexcept_t = typename remove_noexcept<FUNC>::type;
}
