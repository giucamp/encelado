#pragma once
#include <ediacaran/core/ediacaran_common.h>

namespace ediacaran
{
    template <typename FUNC>
        struct remove_noexcept
    {
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
        struct remove_noexcept<RETURN_VALUE(*)(PARAMETERS...)>
    {
        using type = RETURN_VALUE(*)(PARAMETERS...);
    };

    template <typename RETURN_VALUE, typename... PARAMETERS>
        struct remove_noexcept<RETURN_VALUE(*)(PARAMETERS...) noexcept>
    {
        using type = RETURN_VALUE(PARAMETERS...);
    };

    #define IMPLEMENT_REMOVE_NOEXCEPT_MEM(Qualifiers)                                   \
        template <typename CLASS, typename RETURN_VALUE, typename... PARAMETERS>        \
            struct remove_noexcept<RETURN_VALUE (CLASS::*)(PARAMETERS...) Qualifiers>   \
                { using type = RETURN_VALUE(PARAMETERS...); };                          \
        template <typename CLASS, typename RETURN_VALUE, typename... PARAMETERS>        \
            struct remove_noexcept<RETURN_VALUE(CLASS::*)(PARAMETERS...) Qualifiers noexcept>    \
               { using type = RETURN_VALUE(PARAMETERS...); }

    #define NOTHING
    IMPLEMENT_REMOVE_NOEXCEPT_MEM(NOTHING);
    IMPLEMENT_REMOVE_NOEXCEPT_MEM(const);
    IMPLEMENT_REMOVE_NOEXCEPT_MEM(volatile);
    IMPLEMENT_REMOVE_NOEXCEPT_MEM(const volatile);

    template <typename FUNC>
        using remove_noexcept_t = typename remove_noexcept<FUNC>::type;
}