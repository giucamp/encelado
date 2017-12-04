
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2016-2017.

#pragma once
#include "ediacaran/core/char_reader.h"
#include "ediacaran/core/char_writer.h"
#include <memory>

namespace ediacaran
{
    class special_functions
    {
      public:
        using scalar_constructor_function = void (*)(
          void * i_objects_start, void * i_objects_end);
        using scalar_destructor_function = void (*)(void * i_objects_start,
          void * i_objects_end) EDIACARAN_NOEXCEPT_FUNCTION_TYPE;
        using to_chars_function = void (*)(const void * i_source,
          char_writer & i_dest) EDIACARAN_NOEXCEPT_FUNCTION_TYPE;
        using from_chars_function = bool (*)(void * i_dest,
          char_reader & i_source,
          char_writer & i_error_dest) EDIACARAN_NOEXCEPT_FUNCTION_TYPE;

        constexpr special_functions() noexcept = default;

        constexpr special_functions(
          scalar_constructor_function i_scalar_default_constructor,
          scalar_destructor_function i_scalar_destructor,
          to_chars_function i_to_chars, from_chars_function i_from_chars)
            : m_scalar_default_constructor(i_scalar_default_constructor),
              m_scalar_destructor(i_scalar_destructor), m_to_chars(i_to_chars),
              m_from_chars(i_from_chars)
        {
        }

        template <typename TYPE>
        constexpr static special_functions make() noexcept
        {
            return special_functions(make_default_constructor<TYPE>(),
              make_destructor<TYPE>(), make_to_chars<TYPE>(),
              make_from_chars<TYPE>());
        }

        constexpr scalar_constructor_function scalar_default_constructor() const
          noexcept
        {
            return m_scalar_default_constructor;
        }
        constexpr scalar_destructor_function scalar_destructor() const noexcept
        {
            return m_scalar_destructor;
        }
        constexpr to_chars_function to_chars() const noexcept
        {
            return m_to_chars;
        }
        constexpr from_chars_function from_chars() const noexcept
        {
            return m_from_chars;
        }

      private:
        template <typename TYPE>
        static void scalar_default_construct_impl(
          void * i_dest_start, void * i_dest_end)
        {
            std::uninitialized_value_construct(
              static_cast<TYPE *>(i_dest_start),
              static_cast<TYPE *>(i_dest_end));
        }

        template <typename TYPE>
        static void scalar_destroy_impl(void * i_dest_start, void * i_dest_end)
        {
            std::destroy(static_cast<TYPE *>(i_dest_start),
              static_cast<TYPE *>(i_dest_end));
        }

        template <typename TYPE>
        static void to_chars_impl(
          const void * i_source, char_writer & i_dest) noexcept
        {
            i_dest << *static_cast<const TYPE *>(i_source);
        }

        template <typename TYPE>
        static bool try_parse_impl(void * i_dest, char_reader & i_source,
          char_writer & i_error_dest) noexcept
        {
            return try_parse(
              *static_cast<TYPE *>(i_dest), i_source, i_error_dest);
        }

        template <typename TYPE>
        constexpr static scalar_constructor_function
          make_default_constructor() noexcept
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
        constexpr static to_chars_function make_to_chars() noexcept
        {
            if constexpr (has_to_chars_v<TYPE>)
                return &to_chars_impl<TYPE>;
            else
                return nullptr;
        }

        template <typename TYPE>
        constexpr static from_chars_function make_from_chars() noexcept
        {
            if constexpr (has_try_parse_v<TYPE>)
                return &try_parse_impl<TYPE>;
            else
                return nullptr;
        }

      private:
        scalar_constructor_function m_scalar_default_constructor = nullptr;
        scalar_destructor_function m_scalar_destructor = nullptr;
        to_chars_function m_to_chars = nullptr;
        from_chars_function m_from_chars = nullptr;
    };
}
