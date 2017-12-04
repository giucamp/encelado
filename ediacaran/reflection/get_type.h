
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2016-2017.


#pragma once
#include "ediacaran/core/ediacaran_common.h"
#include "ediacaran/reflection/type.h"
#include <type_traits>

namespace ediacaran
{
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

    template <typename TYPE>
    constexpr std::enable_if_t<std::is_arithmetic_v<TYPE>, type_t> create_type(
      tag<TYPE>)
    {
        return create_static_type<TYPE>("arithmetic");
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
