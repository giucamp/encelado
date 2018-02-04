
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2016-2017.

#pragma once
#include "ediacaran/core/address.h"
#include "ediacaran/core/char_reader.h"
#include "ediacaran/core/char_writer.h"
#include <algorithm>
#include <memory>

namespace ediacaran
{
    // trait has_comparer
    template <typename, typename = std::void_t<>> struct has_comparer : std::false_type
    {
    };
    template <typename TYPE>
    struct has_comparer<
      TYPE,
      std::void_t<decltype(
        std::declval<TYPE const &>() < std::declval<TYPE const &>() ||
        std::declval<TYPE const &>() == std::declval<TYPE const &>())>> : std::true_type
    {
    };
    template <typename TYPE> using has_comparer_t          = typename has_comparer<TYPE>::type;
    template <typename TYPE> constexpr bool has_comparer_v = has_comparer<TYPE>::value;

    class special_functions
    {
      public:
        using scalar_constructor_function = void (*)(void * i_dest_start, void * i_dest_end);

        using scalar_destructor_function =
          void (*)(void * i_dest_start, void * i_dest_end) noexcept;

        using scalar_copy_constructor_function =
          void (*)(void * i_dest_start, void * i_dest_end, void const * i_source_start);

        using scalar_move_constructor_function =
          void (*)(void * i_dest_start, void * i_dest_end, void * i_source_start);

        using scalar_copy_assigner_function =
          void (*)(void * i_dest_start, void * i_dest_end, void const * i_source_start);

        using scalar_move_assigner_function =
          void (*)(void * i_dest_start, void * i_dest_end, void * i_source_start);

        using comparer_function = int (*)(void const * i_first, void const * i_second) noexcept;

        using to_chars_function = void (*)(void const * i_source, char_writer & i_dest) noexcept;

        using parse_function =
          expected<void, parse_error> (*)(void * i_dest, char_reader & i_source) noexcept;

        constexpr special_functions() noexcept = default;

        constexpr special_functions(
          scalar_constructor_function      i_scalar_default_constructor,
          scalar_destructor_function       i_scalar_destructor,
          scalar_copy_constructor_function i_scalar_copy_constructor,
          scalar_move_constructor_function i_scalar_move_constructor,
          scalar_copy_assigner_function    i_scalar_copy_assigner,
          scalar_move_assigner_function    i_scalar_move_assigner,
          comparer_function                i_comparer,
          to_chars_function                i_to_chars,
          parse_function               i_parser)
            : m_scalar_default_constructor(i_scalar_default_constructor),
              m_scalar_destructor(i_scalar_destructor),
              m_scalar_copy_constructor(i_scalar_copy_constructor),
              m_scalar_move_constructor(i_scalar_move_constructor),
              m_scalar_copy_assigner(i_scalar_copy_assigner),
              m_scalar_move_assigner(i_scalar_move_assigner), m_comparer(i_comparer),
              m_stringizer(i_to_chars), m_parser(i_parser)
        {
        }

        template <typename TYPE> constexpr static special_functions make() noexcept
        {
            return special_functions(
              make_default_constructor<TYPE>(),
              make_destructor<TYPE>(),
              make_copy_constructor<TYPE>(),
              make_move_constructor<TYPE>(),
              make_copy_assigner<TYPE>(),
              make_move_assigner<TYPE>(),
              make_comparer<TYPE>(),
              make_to_chars<TYPE>(),
              make_parse<TYPE>());
        }

        constexpr auto scalar_default_constructor() const noexcept
        {
            return m_scalar_default_constructor;
        }
        constexpr auto scalar_destructor() const noexcept { return m_scalar_destructor; }
        constexpr auto scalar_copy_constructor() const noexcept
        {
            return m_scalar_copy_constructor;
        }
        constexpr auto scalar_move_constructor() const noexcept
        {
            return m_scalar_move_constructor;
        }
        constexpr auto scalar_copy_assigner() const noexcept { return m_scalar_copy_assigner; }
        constexpr auto scalar_move_assigner() const noexcept { return m_scalar_move_assigner; }
        constexpr auto comparer() const noexcept { return m_comparer; }
        constexpr auto stringizer() const noexcept { return m_stringizer; }
        constexpr auto parser() const noexcept { return m_parser; }

      private:
        template <typename TYPE>
        static TYPE const * get_source_end(
          void * i_dest_start, void * i_dest_end, void const * i_source_start) noexcept
        {
            return static_cast<TYPE const *>(
              address_add(i_source_start, address_diff(i_dest_end, i_dest_start)));
        }

        template <typename TYPE>
        static TYPE const *
          get_source_end(void * i_dest_start, void * i_dest_end, void * i_source_start) noexcept
        {
            return static_cast<TYPE *>(
              address_add(i_source_start, address_diff(i_dest_end, i_dest_start)));
        }

        // function implementations

        template <typename TYPE>
        static void scalar_default_construct_impl(void * i_dest_start, void * i_dest_end)
        {
            std::uninitialized_value_construct(
              static_cast<TYPE *>(i_dest_start), static_cast<TYPE *>(i_dest_end));
        }

        template <typename TYPE>
        static void scalar_destroy_impl(void * i_dest_start, void * i_dest_end) noexcept
        {
            std::destroy(static_cast<TYPE *>(i_dest_start), static_cast<TYPE *>(i_dest_end));
        }

        template <typename TYPE>
        static void scalar_copy_construct_impl(
          void * i_dest_start, void * i_dest_end, void const * i_source_start)
        {
            std::uninitialized_copy(
              static_cast<TYPE const *>(i_source_start),
              get_source_end<TYPE>(i_dest_start, i_dest_end, i_source_start),
              static_cast<TYPE *>(i_dest_start));
        }

        template <typename TYPE>
        static void
          scalar_move_construct_impl(void * i_dest_start, void * i_dest_end, void * i_source_start)
        {
            std::uninitialized_move(
              static_cast<TYPE const *>(i_source_start),
              get_source_end<TYPE>(i_dest_start, i_dest_end, i_source_start),
              static_cast<TYPE *>(i_dest_start));
        }

        template <typename TYPE>
        static void scalar_copy_assign_impl(
          void * i_dest_start, void * i_dest_end, void const * i_source_start)
        {
            std::copy(
              static_cast<TYPE const *>(i_source_start),
              get_source_end<TYPE>(i_dest_start, i_dest_end, i_source_start),
              static_cast<TYPE *>(i_dest_start));
        }

        template <typename TYPE>
        static void
          scalar_move_assign_impl(void * i_dest_start, void * i_dest_end, void * i_source_start)
        {
            std::move(
              static_cast<TYPE const *>(i_source_start),
              get_source_end<TYPE>(i_dest_start, i_dest_end, i_source_start),
              static_cast<TYPE *>(i_dest_start));
        }

        template <typename TYPE>
        static int comparer_impl(void const * i_first, void const * i_second) noexcept
        {
            auto & first  = *static_cast<TYPE const *>(i_first);
            auto & second = *static_cast<TYPE const *>(i_second);
            if (first < second)
                return -1;
            else if (first == second)
                return 0;
            else
                return 1;
        }

        template <typename TYPE>
        static void stringizer_impl(void const * i_source, char_writer & i_dest) noexcept
        {
            i_dest << *static_cast<TYPE const *>(i_source);
        }

        template <typename TYPE>
        static expected<void, parse_error>
          parse_impl(void * i_dest, char_reader & i_source) noexcept
        {
            static_assert(noexcept(parse(*static_cast<TYPE *>(i_dest), i_source)));
            return parse(*static_cast<TYPE *>(i_dest), i_source);
        }

        // function getters

        template <typename TYPE>
        constexpr static scalar_constructor_function make_default_constructor() noexcept
        {
            if constexpr (std::is_default_constructible_v<TYPE>)
                return &scalar_default_construct_impl<TYPE>;
            else
                return nullptr;
        }

        template <typename TYPE>
        constexpr static scalar_destructor_function make_destructor() noexcept
        {
            return &scalar_destroy_impl<TYPE>;
        }

        template <typename TYPE>
        constexpr static scalar_copy_constructor_function make_copy_constructor() noexcept
        {
            if constexpr (std::is_copy_constructible_v<TYPE>)
                return &scalar_copy_construct_impl<TYPE>;
            else
                return nullptr;
        }

        template <typename TYPE>
        constexpr static scalar_move_constructor_function make_move_constructor() noexcept
        {
            if constexpr (std::is_move_constructible_v<TYPE>)
                return &scalar_move_construct_impl<TYPE>;
            else
                return nullptr;
        }

        template <typename TYPE>
        constexpr static scalar_copy_assigner_function make_copy_assigner() noexcept
        {
            if constexpr (std::is_copy_assignable_v<TYPE>)
                return &scalar_copy_assign_impl<TYPE>;
            else
                return nullptr;
        }

        template <typename TYPE>
        constexpr static scalar_move_assigner_function make_move_assigner() noexcept
        {
            if constexpr (std::is_move_assignable_v<TYPE>)
                return &scalar_move_assign_impl<TYPE>;
            else
                return nullptr;
        }

        template <typename TYPE> constexpr static comparer_function make_comparer() noexcept
        {
            if constexpr (has_comparer_v<TYPE>)
                return &comparer_impl<TYPE>;
            else
                return nullptr;
        }

        template <typename TYPE> constexpr static to_chars_function make_to_chars() noexcept
        {
            if constexpr (is_stringizable_v<TYPE>)
                return &stringizer_impl<TYPE>;
            else
                return nullptr;
        }

        template <typename TYPE> constexpr static parse_function make_parse() noexcept
        {
            if constexpr (has_parse_v<TYPE>)
                return &parse_impl<TYPE>;
            else
                return nullptr;
        }

      private:
        scalar_constructor_function      m_scalar_default_constructor = nullptr;
        scalar_destructor_function       m_scalar_destructor          = nullptr;
        scalar_copy_constructor_function m_scalar_copy_constructor    = nullptr;
        scalar_move_constructor_function m_scalar_move_constructor    = nullptr;
        scalar_copy_assigner_function    m_scalar_copy_assigner       = nullptr;
        scalar_move_assigner_function    m_scalar_move_assigner       = nullptr;
        comparer_function                m_comparer                   = nullptr;
        to_chars_function                m_stringizer                 = nullptr;
        parse_function                   m_parser                 = nullptr;
    };
}
