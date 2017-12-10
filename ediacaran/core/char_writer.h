
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2016-2017.


#pragma once
#include "ediacaran/core/ediacaran_common.h"
#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace ediacaran
{
    struct SpacesTag
    {
        constexpr SpacesTag() {}
    };

    constexpr SpacesTag spaces;

    class char_writer
    {
      public:
        constexpr char_writer() noexcept {}

        constexpr char_writer(char * i_dest, size_t i_size) noexcept
            : m_curr_char(i_dest), m_remaining_size(static_cast<ptrdiff_t>(i_size - 1))
        {
            EDIACARAN_ASSERT(i_size > 0);
            *m_curr_char = 0;
        }

        template <size_t SIZE> constexpr char_writer(char (&i_dest)[SIZE]) noexcept : char_writer(i_dest, SIZE)
        {
            static_assert(SIZE > 0);
        }

        constexpr char_writer(const char_writer &) noexcept = default;

        constexpr char_writer & operator=(const char_writer &) noexcept = default;

        constexpr ptrdiff_t remaining_size() const noexcept { return m_remaining_size; }

        constexpr char_writer & operator<<(char i_char) noexcept
        {
            if (--m_remaining_size >= 0)
            {
                *m_curr_char++ = i_char;
                *m_curr_char = 0;
            }
            return *this;
        }

        constexpr char_writer & operator<<(SpacesTag) noexcept { return operator<<(' '); }

        constexpr char_writer & operator<<(const string_view & i_string) noexcept
        {
            auto const length_to_write = std::min(m_remaining_size, static_cast<ptrdiff_t>(i_string.length()));

            m_remaining_size -= static_cast<ptrdiff_t>(i_string.length());

            if (length_to_write > 0)
            {
                for (ptrdiff_t index = 0; index < length_to_write; index++)
                    m_curr_char[index] = i_string[index];
                m_curr_char += length_to_write;
                *m_curr_char = 0;
            }
            return *this;
        }

        template <typename... TYPE> constexpr void write(TYPE &&... i_args) noexcept
        {
            int dummy[sizeof...(TYPE)] = {(*this << std::forward<TYPE>(i_args), 0)...};
            (void)dummy;
        }

        constexpr char * next_dest() const noexcept { return m_curr_char; }

      private:
        char * m_curr_char = nullptr;
        ptrdiff_t m_remaining_size = 0;
    };

    template <typename UINT_TYPE>
    constexpr std::enable_if_t<
      std::is_integral_v<UINT_TYPE> && !std::is_signed_v<UINT_TYPE> && !std::is_same_v<UINT_TYPE, bool>, char_writer> &
      operator<<(char_writer & i_dest, UINT_TYPE i_source) noexcept
    {
        constexpr UINT_TYPE ten = 10;

        constexpr int buffer_size = std::numeric_limits<UINT_TYPE>::digits10 + 1;
        char buffer[buffer_size] = {};
        size_t length = 0;
        do
        {
            buffer[length] = static_cast<char>('0' + i_source % ten);
            i_source /= ten;

            EDIACARAN_INTERNAL_ASSERT(length < buffer_size); // buffer too small?
            length++;

        } while (i_source > 0);

        // std::reverse is not constexpr
        for (size_t index = 0; index < length / 2; index++)
        {
            auto const other_index = (length - 1) - index;
            auto tmp = buffer[index];
            buffer[index] = buffer[other_index];
            buffer[other_index] = tmp;
        }

        i_dest << string_view(buffer, length);

        return i_dest;
    }

    template <typename SINT_TYPE>
    constexpr std::enable_if_t<
      std::is_integral_v<SINT_TYPE> && std::is_signed_v<SINT_TYPE> && !std::is_same_v<SINT_TYPE, bool>, char_writer> &
      operator<<(char_writer & i_dest, SINT_TYPE i_source) noexcept
    {
        const bool is_negative = i_source < 0;

        constexpr SINT_TYPE ten = 10;

        constexpr int buffer_size = std::numeric_limits<SINT_TYPE>::digits10 + 1;
        char buffer[buffer_size] = {};
        int length = 0;
        /* note: if the number is negative, we can't just negate the sign and use the same algorithm,
			because the unary minus operator is lossy: for example, negating -128 as int8 produces an overflow, as 
			128 can't be represented as int8 */
        if (is_negative)
        {
            do
            {

                /* note: we do not use the modulo operator %, because it has implementation-defined
					behavior with non-positive operands. */
                SINT_TYPE const new_value = i_source / ten;
                buffer[length] = static_cast<char>('0' + new_value * ten - i_source);
                i_source = new_value;
                length++;

                EDIACARAN_INTERNAL_ASSERT(length < buffer_size || i_source == 0); // buffer too small?
            } while (i_source != 0);
        }
        else
        {
            do
            {

                buffer[length] = static_cast<char>('0' + i_source % ten);
                length++;
                i_source /= ten;

                EDIACARAN_INTERNAL_ASSERT(length < buffer_size || i_source == 0); // buffer too small?
            } while (i_source != 0);
        }

        if (is_negative)
        {
            i_dest << '-';
        }

        // std::reverse is not constexpr
        for (size_t index = 0; index < length / 2; index++)
        {
            auto const other_index = (length - 1) - index;
            auto tmp = buffer[index];
            buffer[index] = buffer[other_index];
            buffer[other_index] = tmp;
        }

        i_dest << string_view(buffer, length);

        return i_dest;
    }

    template <typename BOOL>
    constexpr std::enable_if_t<std::is_same_v<BOOL, bool>, char_writer &> operator<<(
      char_writer & i_dest, BOOL i_value) noexcept
    {
        return i_dest << (i_value ? "true" : "false");
    }

    char_writer & operator<<(char_writer & i_dest, float i_value);
    char_writer & operator<<(char_writer & i_dest, double i_value);
    char_writer & operator<<(char_writer & i_dest, long double i_value);

    // trait has_to_chars
    template <typename, typename = std::void_t<>> struct has_to_chars : std::false_type
    {
    };
    template <typename TYPE>
    struct has_to_chars<TYPE, std::void_t<decltype(std::declval<char_writer &>() << std::declval<const TYPE &>())>>
        : std::true_type
    {
    };
    template <typename TYPE> using has_to_chars_t = typename has_to_chars<TYPE>::type;
    template <typename TYPE> constexpr bool has_to_chars_v = has_to_chars<TYPE>::value;

    /** Returns the number of bytes required by the char array (including the null terminator) */
    template <size_t SIZE, typename... TYPE>
    constexpr size_t to_chars(char (&o_char_array)[SIZE], const TYPE &... i_objects)
    {
        char * const dest = o_char_array;
        return to_chars(dest, SIZE, i_objects...);
    }

    /** Returns the number of bytes required by the char array (including the null terminator) */
    template <typename... TYPE>
    constexpr size_t to_chars(char * o_char_array, size_t i_array_size, const TYPE &... i_objects)
    {
        char_writer writer(o_char_array, i_array_size);
        (writer << ... << i_objects);
        return static_cast<size_t>(static_cast<ptrdiff_t>(i_array_size) - writer.remaining_size());
    }
}
