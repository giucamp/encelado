
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2016-2017.


#pragma once
#include "ediacaran/core/constexpr_string.h"
#include "ediacaran/core/ediacaran_common.h"
#include "ediacaran/reflection/special_functions.h"
#include <limits>
#include <type_traits>

namespace ediacaran
{
    class symbol_t
    {
      public:
        constexpr symbol_t(const char * const i_name) noexcept : m_name(i_name) {}
        constexpr string_view name() const
          noexcept // workaround for gcc considering the comparison of two char* non-constexpr
        {
            return string_view(m_name, string_view::traits_type::length(m_name));
        }

      private:
        const char * const m_name;
    };

    enum class type_kind
    {
        is_fundamental,
        is_class,
        is_enum
    };

    class type_t : public symbol_t
    {
      public:
        constexpr type_t(type_kind i_kind, const char * const i_name, size_t i_size, size_t i_alignment,
          const ediacaran::special_functions & i_special_functions) noexcept
            : symbol_t(i_name), m_size(i_size), m_alignment(i_alignment), m_kind(i_kind),
              m_special_functions(i_special_functions)
        {
        }

        constexpr size_t size() const noexcept { return m_size; }

        constexpr size_t alignment() const noexcept { return m_alignment; }

        constexpr bool is_fundamental() const noexcept { return m_kind == type_kind::is_fundamental; }

        constexpr bool is_class() const noexcept { return m_kind == type_kind::is_class; }

        constexpr bool is_enum() const noexcept { return m_kind == type_kind::is_enum; }

        constexpr bool is_constructible() const noexcept
        {
            return m_special_functions.scalar_default_constructor() != nullptr;
        }

        constexpr bool is_destructible() const noexcept { return m_special_functions.scalar_destructor() != nullptr; }

        constexpr bool is_copy_constructible() const noexcept
        {
            return m_special_functions.scalar_copy_constructor() != nullptr;
        }

        constexpr bool is_move_constructible() const noexcept
        {
            return m_special_functions.scalar_move_constructor() != nullptr;
        }

        constexpr bool is_copy_assignable() const noexcept
        {
            return m_special_functions.scalar_copy_assigner() != nullptr;
        }

        constexpr bool is_move_assignable() const noexcept
        {
            return m_special_functions.scalar_move_assigner() != nullptr;
        }

        constexpr bool is_comparable() const noexcept { return m_special_functions.comparer() != nullptr; }

        constexpr bool is_stringizable() const noexcept { return m_special_functions.stringizer() != nullptr; }

        constexpr bool is_parsable() const noexcept { return m_special_functions.try_parser() != nullptr; }


        // special functions

        void construct(void * i_dest) const
        {
            EDIACARAN_ASSERT(i_dest != nullptr);
            m_special_functions.scalar_default_constructor()(i_dest, address_add(i_dest, m_size));
        }

        void copy_construct(void * i_dest, const void * i_source) const
        {
            EDIACARAN_ASSERT(i_dest != nullptr && i_source != nullptr);
            m_special_functions.scalar_copy_constructor()(i_dest, address_add(i_dest, m_size), i_source);
        }

        void move_construct(void * i_dest, void * i_source) const
        {
            EDIACARAN_ASSERT(i_dest != nullptr && i_source != nullptr);
            m_special_functions.scalar_move_constructor()(i_dest, address_add(i_dest, m_size), i_source);
        }

        void copy_assign(void * i_dest, const void * i_source) const
        {
            EDIACARAN_ASSERT(i_dest != nullptr && i_source != nullptr);
            m_special_functions.scalar_copy_assigner()(i_dest, address_add(i_dest, m_size), i_source);
        }

        void move_assign(void * i_dest, void * i_source) const
        {
            EDIACARAN_ASSERT(i_dest != nullptr && i_source != nullptr);
            m_special_functions.scalar_move_assigner()(i_dest, address_add(i_dest, m_size), i_source);
        }

        void destroy(void * i_dest) const noexcept
        {
            EDIACARAN_ASSERT(i_dest != nullptr);
            m_special_functions.scalar_destructor()(i_dest, address_add(i_dest, m_size));
        }

        int compare(void const * i_first, void const * i_second) const noexcept
        {
            EDIACARAN_ASSERT(i_first != nullptr && i_second != nullptr);
            return m_special_functions.comparer()(i_first, i_second);
        }

        bool compare_equal(void const * i_first, void const * i_second) const noexcept
        {
            return compare(i_first, i_second) == 0;
        }

        bool compare_less(void const * i_first, void const * i_second) const noexcept
        {
            return compare(i_first, i_second) < 0;
        }

        void stringize(const void * i_source, char_writer & i_dest) const noexcept
        {
            EDIACARAN_ASSERT(i_source != nullptr);
            (*m_special_functions.stringizer())(i_source, i_dest);
        }

        bool try_parse(void * i_dest, char_reader & i_source, char_writer & i_error_dest) const noexcept
        {
            EDIACARAN_ASSERT(i_dest != nullptr);
            return (*m_special_functions.try_parser())(i_dest, i_source, i_error_dest);
        }

        bool try_parse(void * i_dest, string_view const & i_source, char_writer & i_error_dest) const noexcept
        {
            char_reader source(i_source);
            return try_parse(i_dest, source, i_error_dest);
        }

      private:
        size_t const m_size;
        size_t const m_alignment;
        type_kind const m_kind;
        ediacaran::special_functions const m_special_functions;
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

    template <typename TYPE> constexpr type_t make_fundamental_type(const char * i_name) noexcept
    {
        return type_t{
          type_kind::is_fundamental, i_name, sizeof(TYPE), alignof(TYPE), special_functions::template make<TYPE>()};
    }

    constexpr type_t create_type(tag<bool>) noexcept { return make_fundamental_type<bool>("bool"); }

    constexpr type_t create_type(tag<char>) noexcept { return make_fundamental_type<bool>("char"); }

    constexpr type_t create_type(tag<float>) noexcept { return make_fundamental_type<float>("float"); }

    constexpr type_t create_type(tag<double>) noexcept { return make_fundamental_type<double>("double"); }

    constexpr type_t create_type(tag<long double>) noexcept { return make_fundamental_type<double>("long_double"); }

    template <typename INT_TYPE>
    constexpr std::enable_if_t<std::is_integral_v<INT_TYPE> && !std::is_same_v<INT_TYPE, bool>, type_t> create_type(
      tag<INT_TYPE>) noexcept
    {
        return make_fundamental_type<INT_TYPE>(constexpr_string<detail::WriteIntTypeName<INT_TYPE>>::string.data());
    }

    constexpr type_t create_type(tag<void *>) noexcept { return make_fundamental_type<void *>("pointer"); }

    constexpr type_t create_type(tag<void>) noexcept
    {
        return type_t{type_kind::is_fundamental, "void", 1, 1, special_functions{}};
    }

    namespace detail
    {
        template <typename TYPE> constexpr type_t s_type{create_type(tag<TYPE>{})};
    }

    template <typename TYPE, typename = std::enable_if_t<!(std::is_class_v<TYPE> || std::is_enum_v<TYPE>)>>
    constexpr const type_t & get_type() noexcept
    {
        return detail::s_type<TYPE>;
    }
}
