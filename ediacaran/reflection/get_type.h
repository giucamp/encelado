
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2016-2017.


#pragma once
#include "ediacaran/core/ediacaran_common.h"
#include "ediacaran/core/char_writer.h"
#include "ediacaran/core/constexpr_string.h"
#include "ediacaran/reflection/type.h"
#include <type_traits>
#include <limits>
#include <array>

namespace ediacaran
{
    namespace detail
    {
        template <typename INT_TYPE>
            struct WriteIntTypeName
        {
            constexpr void operator ()(char_writer & o_dest) noexcept
            {
                static_assert(std::numeric_limits<INT_TYPE>::radix == 2);

                if constexpr(!std::is_signed_v<INT_TYPE>)
                    o_dest << 'u';
                o_dest << "int" << std::numeric_limits<INT_TYPE>::digits;
            }
        };
    }

    template <typename> struct tag
    {
    };

    template <typename TYPE>
    constexpr type_t create_static_type(const char * i_name)
    {
        return type_t{i_name, sizeof(TYPE), alignof(TYPE),
          special_functions::template make<TYPE>()};
    }

    constexpr type_t create_type(tag<bool>)
    {
        return create_static_type<bool>("bool");
    }

    constexpr type_t create_type(tag<float>)
    {
        return create_static_type<float>("float");
    }

    constexpr type_t create_type(tag<double>)
    {
        return create_static_type<double>("double");
    }

    constexpr type_t create_type(tag<long double>)
    {
        return create_static_type<double>("long double");
    }

    template <typename INT_TYPE>
    constexpr std::enable_if_t<std::is_integral_v<INT_TYPE> && !std::is_same_v<INT_TYPE, bool>, type_t> create_type(
      tag<INT_TYPE>)
    {
        return create_static_type<INT_TYPE>(constexpr_string<detail::WriteIntTypeName<INT_TYPE>>::string.data());
    }

    constexpr type_t create_type(tag<void *>)
    {
        return create_static_type<void *>("pointer");
    }

    constexpr type_t create_type(tag<void>)
    {
        return type_t{"void", 1, 1, special_functions{}};
    }

    namespace detail
    {
        template <typename TYPE>
        constexpr type_t s_type{create_type(tag<TYPE>{})};
    }

    template <typename TYPE> constexpr const type_t & get_naked_type()
    {
        return detail::s_type<TYPE>;
    }
}
