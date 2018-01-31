#pragma once

#include "ediacaran/core/constexpr_string.h"
#include "ediacaran/core/remove_noexcept.h"
#include "ediacaran/reflection/class_template_specialization.h"
#include "ediacaran/reflection/class_type.h"
#include "ediacaran/reflection/enum_type.h"
#include "ediacaran/reflection/qualified_type_ptr.h"
#include <ediacaran/core/array.h>

namespace ediacaran
{
    namespace detail
    {
        template <typename INT_TYPE> struct WriteIntTypeName
        {
            constexpr void operator()(char_writer & o_dest) noexcept
            {
                static_assert(std::numeric_limits<INT_TYPE>::radix == 2);
                if constexpr (!std::is_signed_v<INT_TYPE>)
                    o_dest << "uint" << std::numeric_limits<INT_TYPE>::digits;
                else
                    o_dest << "int" << (std::numeric_limits<INT_TYPE>::digits + 1);
            }
        };

        template <typename TYPE> constexpr type MakeFundamentalType(const char * i_name) noexcept
        {
            return type{type_kind::is_fundamental,
                        i_name,
                        sizeof(TYPE),
                        alignof(TYPE),
                        special_functions::template make<TYPE>()};
        }

        template <typename, typename> struct TypeInstance;

        constexpr type g_type_bool{MakeFundamentalType<bool>("bool")};
        constexpr type g_type_char{MakeFundamentalType<char>("char")};
        constexpr type g_type_float{MakeFundamentalType<float>("float")};
        constexpr type g_type_pointer{MakeFundamentalType<void *>("pointer")};
        constexpr type g_type_double{MakeFundamentalType<double>("double")};
        constexpr type g_type_long_double{MakeFundamentalType<long double>("long_double")};
        constexpr type g_type_void{type_kind::is_fundamental, "void", 1, 1, special_functions{}};

        template <> struct TypeInstance<bool, bool>
        {
            constexpr static const auto & instance() noexcept { return g_type_bool; }
        };

        template <> struct TypeInstance<char, char>
        {
            constexpr static const auto & instance() noexcept { return g_type_char; }
        };

        template <> struct TypeInstance<float, float>
        {
            constexpr static const auto & instance() noexcept { return g_type_float; }
        };

        template <> struct TypeInstance<double, double>
        {
            constexpr static const auto & instance() noexcept { return g_type_double; }
        };

        template <> struct TypeInstance<long double, long double>
        {
            constexpr static const auto & instance() noexcept { return g_type_long_double; }
        };

        template <> struct TypeInstance<void *, void *>
        {
            constexpr static const auto & instance() noexcept { return g_type_pointer; }
        };

        template <> struct TypeInstance<void, void>
        {
            constexpr static const auto & instance() noexcept { return g_type_void; }
        };

        template <typename INT_TYPE>
        struct TypeInstance<
          INT_TYPE,
          std::
            enable_if_t<std::is_integral_v<INT_TYPE> && !std::is_same_v<INT_TYPE, bool>, INT_TYPE>>
        {
            constexpr static type         s_instance{MakeFundamentalType<INT_TYPE>(
              constexpr_string<WriteIntTypeName<INT_TYPE>>::string.data())};
            constexpr static const auto & instance() noexcept { return s_instance; }
        };

    } // namespace detail
} // namespace ediacaran

#include "ediacaran/reflection/detail/class_reflection.h"
#include "ediacaran/reflection/detail/container_reflection.h"
#include "ediacaran/reflection/detail/enum_reflection.h"

namespace ediacaran
{
    template <typename TYPE> constexpr const auto & get_type() noexcept
    {
        return detail::TypeInstance<TYPE, TYPE>::instance();
    }

    constexpr const type & get_ptr_type() noexcept
    {
        return detail::TypeInstance<void *, void *>::instance();
    }

    template <typename TYPE> constexpr const class_type & get_class_type() noexcept
    {
        return detail::TypeInstance<TYPE, TYPE>::instance();
    }

    template <typename TYPE>
    constexpr const enum_type<std::underlying_type_t<TYPE>> & get_enum_type() noexcept
    {
        return detail::TypeInstance<TYPE, TYPE>::instance();
    }

    /** Retrieves (by value) a qualified_type_ptr associated to the template argument.
    The result is never empty (is_empty() always return false). The template
    argument may be void or any void pointer (with any cv-qualification). */
    template <typename TYPE> constexpr qualified_type_ptr get_qualified_type()
    {
        static_assert(
          detail::StaticQualification<TYPE>::s_indirection_levels <=
            qualified_type_ptr::s_max_indirection_levels,
          "Maximum indirection level exceeded");

        return qualified_type_ptr(
          &get_type<typename detail::StaticQualification<TYPE>::UnderlyingType>(),
          detail::StaticQualification<TYPE>::s_indirection_levels,
          detail::StaticQualification<TYPE>::s_constness_word,
          detail::StaticQualification<TYPE>::s_volatileness_word);
    }

    constexpr auto reflect(string_view ** i_ptr)
    {
        char const class_name[] = "ediacaran::string_view";
        using bases             = type_list<>;

        using namespace ediacaran;
        using this_class = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties = make_array(REFL_ACCESSOR_RO_PROP("size", size));

        return make_class<this_class, bases>(class_name, properties);
    }

    constexpr auto reflect(type ** i_ptr)
    {
        char const class_name[] = "ediacaran::type";

        using namespace ediacaran;
        using this_class = std::remove_reference_t<decltype(**i_ptr)>;

        auto const properties = make_array(
          REFL_ACCESSOR_RO_PROP("size", size),
          REFL_ACCESSOR_RO_PROP("alignment", alignment),
          REFL_ACCESSOR_RO_PROP("is_constructible", is_constructible),
          REFL_ACCESSOR_RO_PROP("is_destructible", is_destructible),
          REFL_ACCESSOR_RO_PROP("is_copy_constructible", is_copy_constructible),
          REFL_ACCESSOR_RO_PROP("is_move_constructible", is_move_constructible),
          REFL_ACCESSOR_RO_PROP("is_copy_assignable", is_copy_assignable),
          REFL_ACCESSOR_RO_PROP("is_move_assignable", is_move_assignable),
          REFL_ACCESSOR_RO_PROP("is_comparable", is_comparable),
          REFL_ACCESSOR_RO_PROP("is_stringizable", is_stringizable),
          REFL_ACCESSOR_RO_PROP("is_parsable", is_parsable));

        return make_class<this_class>(class_name, properties);
    }

    constexpr auto reflect(qualified_type_ptr ** i_ptr)
    {
        char const class_name[] = "ediacaran::qualified_type_ptr";
        using this_class        = std::remove_reference_t<decltype(**i_ptr)>;
        return make_class<this_class>(class_name);
    }

    constexpr auto reflect(cv_qualification ** i_ptr)
    {
        using this_enum = std::remove_reference_t<decltype(**i_ptr)>;
        return make_enum<this_enum>(
          "ediacaran::cv_qualification",
          make_array(
            make_enum_member("const", this_enum::const_q),
            make_enum_member("volatile", this_enum::volatile_q)));
    }
}
