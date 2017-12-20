
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2016-2017.


#pragma once
#include "ediacaran/core/constexpr_string.h"
#include "ediacaran/core/ediacaran_common.h"
#include "ediacaran/reflection/special_functions.h"
#include <limits>

namespace ediacaran
{
    class symbol_t
    {
      public:
        constexpr symbol_t(const char * const i_name) noexcept : m_name(i_name) {}
        constexpr string_view name() const noexcept // workaround for gcc considering the comparison of two char* non-constexpr
            { return string_view(m_name, string_view::traits_type::length(m_name)); }

      private:
        const char * const m_name;
    };

    class type_t : public symbol_t
    {
      public:
        constexpr type_t(const char * const i_name, size_t i_size, size_t i_alignment,
          const ediacaran::special_functions & i_special_functions) noexcept
            : symbol_t(i_name), m_size(i_size), m_alignment(i_alignment), m_special_functions(i_special_functions)
        {
        }

        constexpr size_t size() const noexcept { return m_size; }

        constexpr size_t alignment() const noexcept { return m_alignment; }


        // special functions

        void construct(void * i_dest) const
        {
            m_special_functions.scalar_default_constructor()(i_dest, address_add(i_dest, m_size));
        }

        void copy_construct(void * i_dest, const void * i_source) const
        {
            m_special_functions.scalar_copy_constructor()(i_dest, address_add(i_dest, m_size), i_source);
        }

        void move_construct(void * i_dest, void * i_source) const
        {
            m_special_functions.scalar_move_constructor()(i_dest, address_add(i_dest, m_size), i_source);
        }

        void copy_assign(void * i_dest, const void * i_source) const
        {
            m_special_functions.scalar_copy_assigner()(i_dest, address_add(i_dest, m_size), i_source);
        }

        void move_assign(void * i_dest, void * i_source) const
        {
            m_special_functions.scalar_move_assigner()(i_dest, address_add(i_dest, m_size), i_source);
        }

        void destroy(void * i_dest) const
        {
            m_special_functions.scalar_destructor()(i_dest, address_add(i_dest, m_size));
        }

        void to_chars(const void * i_source, char_writer & i_dest) const noexcept
        {
            (*m_special_functions.to_chars())(i_source, i_dest);
        }

        bool from_chars(void * i_dest, char_reader & i_source, char_writer & i_error_dest) const noexcept
        {
            return (*m_special_functions.from_chars())(i_dest, i_source, i_error_dest);
        }

      private:
        size_t const m_size;
        size_t const m_alignment;
        ediacaran::special_functions m_special_functions;
    };

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
    }

    template <typename> struct tag
    {
    };

    template <typename TYPE> constexpr type_t create_static_type(const char * i_name) noexcept
    {
        return type_t{i_name, sizeof(TYPE), alignof(TYPE), special_functions::template make<TYPE>()};
    }

    constexpr type_t create_type(tag<bool>) noexcept { return create_static_type<bool>("bool"); }

    constexpr type_t create_type(tag<char>) noexcept { return create_static_type<bool>("char"); }

    constexpr type_t create_type(tag<float>) noexcept { return create_static_type<float>("float"); }

    constexpr type_t create_type(tag<double>) noexcept { return create_static_type<double>("double"); }

    constexpr type_t create_type(tag<long double>) noexcept { return create_static_type<double>("long double"); }

    template <typename INT_TYPE>
    constexpr std::enable_if_t<std::is_integral_v<INT_TYPE> && !std::is_same_v<INT_TYPE, bool>, type_t> create_type(
      tag<INT_TYPE>) noexcept
    {
        return create_static_type<INT_TYPE>(constexpr_string<detail::WriteIntTypeName<INT_TYPE>>::string.data());
    }

    constexpr type_t create_type(tag<void *>) noexcept { return create_static_type<void *>("pointer"); }

    constexpr type_t create_type(tag<void>) noexcept { return type_t{"void", 1, 1, special_functions{}}; }

    namespace detail
    {
        template <typename TYPE> constexpr type_t s_type{create_type(tag<TYPE>{})};
    }

    template <typename TYPE, typename = std::enable_if_t<!(std::is_class_v<TYPE> || std::is_enum_v<TYPE>)>>
    constexpr const type_t & get_naked_type() noexcept
    {
        return detail::s_type<TYPE>;
    }
}
