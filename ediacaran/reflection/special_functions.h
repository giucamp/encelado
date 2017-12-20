
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2016-2017.

#pragma once
#include "ediacaran/core/address.h"
#include "ediacaran/core/char_reader.h"
#include "ediacaran/core/char_writer.h"
#include <algorithm>
#include <memory>

namespace ediacaran
{
    class special_functions
    {
      public:
        using scalar_constructor_function = void (*)(void * i_dest_start, void * i_dest_end);

        using scalar_destructor_function = void (*)(
          void * i_dest_start, void * i_dest_end) EDIACARAN_NOEXCEPT_FUNCTION_TYPE;

        using scalar_copy_constructor_function = void (*)(
          void * i_dest_start, void * i_dest_end, const void * i_source_start);

        using scalar_move_constructor_function = void (*)(
          void * i_dest_start, void * i_dest_end, void * i_source_start);

        using scalar_copy_assigner_function = void (*)(
          void * i_dest_start, void * i_dest_end, const void * i_source_start);

        using scalar_move_assigner_function = void (*)(void * i_dest_start, void * i_dest_end, void * i_source_start);

        using to_chars_function = void (*)(
          const void * i_source, char_writer & i_dest) EDIACARAN_NOEXCEPT_FUNCTION_TYPE;

        using from_chars_function = bool (*)(
          void * i_dest, char_reader & i_source, char_writer & i_error_dest) EDIACARAN_NOEXCEPT_FUNCTION_TYPE;

        constexpr special_functions() noexcept = default;

        constexpr special_functions(scalar_constructor_function i_scalar_default_constructor,
          scalar_destructor_function i_scalar_destructor, scalar_copy_constructor_function i_scalar_copy_constructor,
          scalar_move_constructor_function i_scalar_move_constructor,
          scalar_copy_assigner_function i_scalar_copy_assigner, scalar_move_assigner_function i_scalar_move_assigner,
          to_chars_function i_to_chars, from_chars_function i_from_chars)
            : m_scalar_default_constructor(i_scalar_default_constructor), m_scalar_destructor(i_scalar_destructor),
              m_scalar_copy_constructor(i_scalar_copy_constructor),
              m_scalar_move_constructor(i_scalar_move_constructor), m_scalar_copy_assigner(i_scalar_copy_assigner),
              m_scalar_move_assigner(i_scalar_move_assigner), m_to_chars(i_to_chars), m_from_chars(i_from_chars)
        {
        }

        template <typename TYPE> constexpr static special_functions make() noexcept
        {
            return special_functions(make_default_constructor<TYPE>(), make_destructor<TYPE>(),
              make_copy_constructor<TYPE>(), make_move_constructor<TYPE>(), make_copy_assigner<TYPE>(),
              make_move_assigner<TYPE>(), make_to_chars<TYPE>(), make_from_chars<TYPE>());
        }

        constexpr auto scalar_default_constructor() const noexcept { return m_scalar_default_constructor; }
        constexpr auto scalar_destructor() const noexcept { return m_scalar_destructor; }
        constexpr auto scalar_copy_constructor() const noexcept { return m_scalar_copy_constructor; }
        constexpr auto scalar_move_constructor() const noexcept { return m_scalar_move_constructor; }
        constexpr auto scalar_copy_assigner() const noexcept { return m_scalar_copy_assigner; }
        constexpr auto scalar_move_assigner() const noexcept { return m_scalar_move_assigner; }
        constexpr auto to_chars() const noexcept { return m_to_chars; }
        constexpr auto from_chars() const noexcept { return m_from_chars; }

      private:
        template <typename TYPE>
        static const TYPE * get_source_end(void * i_dest_start, void * i_dest_end, const void * i_source_start) noexcept
        {
            return static_cast<const TYPE *>(address_add(i_source_start, address_diff(i_dest_end, i_dest_start)));
        }

        template <typename TYPE>
        static const TYPE * get_source_end(void * i_dest_start, void * i_dest_end, void * i_source_start) noexcept
        {
            return static_cast<TYPE *>(address_add(i_source_start, address_diff(i_dest_end, i_dest_start)));
        }

        // function implementations

        template <typename TYPE> static void scalar_default_construct_impl(void * i_dest_start, void * i_dest_end)
        {
            std::uninitialized_value_construct(static_cast<TYPE *>(i_dest_start), static_cast<TYPE *>(i_dest_end));
        }

        template <typename TYPE> static void scalar_destroy_impl(void * i_dest_start, void * i_dest_end) noexcept
        {
            std::destroy(static_cast<TYPE *>(i_dest_start), static_cast<TYPE *>(i_dest_end));
        }

        template <typename TYPE>
        static void scalar_copy_construct_impl(void * i_dest_start, void * i_dest_end, const void * i_source_start)
        {
            std::uninitialized_copy(static_cast<const TYPE *>(i_source_start),
              get_source_end<TYPE>(i_dest_start, i_dest_end, i_source_start), static_cast<TYPE *>(i_dest_start));
        }

        template <typename TYPE>
        static void scalar_move_construct_impl(void * i_dest_start, void * i_dest_end, void * i_source_start)
        {
            std::uninitialized_move(static_cast<const TYPE *>(i_source_start),
              get_source_end<TYPE>(i_dest_start, i_dest_end, i_source_start), static_cast<TYPE *>(i_dest_start));
        }

        template <typename TYPE>
        static void scalar_copy_assign_impl(void * i_dest_start, void * i_dest_end, const void * i_source_start)
        {
            std::copy(static_cast<const TYPE *>(i_source_start),
              get_source_end<TYPE>(i_dest_start, i_dest_end, i_source_start), static_cast<TYPE *>(i_dest_start));
        }

        template <typename TYPE>
        static void scalar_move_assign_impl(void * i_dest_start, void * i_dest_end, void * i_source_start)
        {
            std::move(static_cast<const TYPE *>(i_source_start),
              get_source_end<TYPE>(i_dest_start, i_dest_end, i_source_start), static_cast<TYPE *>(i_dest_start));
        }

        template <typename TYPE> static void to_chars_impl(const void * i_source, char_writer & i_dest) noexcept
        {
            i_dest << *static_cast<const TYPE *>(i_source);
        }

        template <typename TYPE>
        static bool try_parse_impl(void * i_dest, char_reader & i_source, char_writer & i_error_dest) noexcept
        {
            return try_parse(*static_cast<TYPE *>(i_dest), i_source, i_error_dest);
        }

        // function getters

        template <typename TYPE> constexpr static scalar_constructor_function make_default_constructor() noexcept
        {
            if constexpr (std::is_default_constructible_v<TYPE>)
                return &scalar_default_construct_impl<TYPE>;
            else
                return nullptr;
        }

        template <typename TYPE> constexpr static scalar_destructor_function make_destructor() noexcept
        {
            return &scalar_destroy_impl<TYPE>;
        }

        template <typename TYPE> constexpr static scalar_copy_constructor_function make_copy_constructor() noexcept
        {
            if constexpr (std::is_copy_constructible_v<TYPE>)
                return &scalar_copy_construct_impl<TYPE>;
            else
                return nullptr;
        }

        template <typename TYPE> constexpr static scalar_move_constructor_function make_move_constructor() noexcept
        {
            if constexpr (std::is_move_constructible_v<TYPE>)
                return &scalar_move_construct_impl<TYPE>;
            else
                return nullptr;
        }

        template <typename TYPE> constexpr static scalar_copy_assigner_function make_copy_assigner() noexcept
        {
            if constexpr (std::is_copy_assignable_v<TYPE>)
                return &scalar_copy_construct_impl<TYPE>;
            else
                return nullptr;
        }

        template <typename TYPE> constexpr static scalar_move_assigner_function make_move_assigner() noexcept
        {
            if constexpr (std::is_move_assignable_v<TYPE>)
                return &scalar_move_construct_impl<TYPE>;
            else
                return nullptr;
        }

        template <typename TYPE> constexpr static to_chars_function make_to_chars() noexcept
        {
            if constexpr (has_to_chars_v<TYPE>)
                return &to_chars_impl<TYPE>;
            else
                return nullptr;
        }

        template <typename TYPE> constexpr static from_chars_function make_from_chars() noexcept
        {
            if constexpr (has_try_parse_v<TYPE>)
                return &try_parse_impl<TYPE>;
            else
                return nullptr;
        }

      private:
        scalar_constructor_function m_scalar_default_constructor = nullptr;
        scalar_destructor_function m_scalar_destructor = nullptr;
        scalar_copy_constructor_function m_scalar_copy_constructor = nullptr;
        scalar_move_constructor_function m_scalar_move_constructor = nullptr;
        scalar_copy_assigner_function m_scalar_copy_assigner = nullptr;
        scalar_move_assigner_function m_scalar_move_assigner = nullptr;
        to_chars_function m_to_chars = nullptr;
        from_chars_function m_from_chars = nullptr;
    };
}
