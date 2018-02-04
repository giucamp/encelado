
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2016-2017.


#pragma once
#include "ediacaran/core/char_writer.h"
#include "ediacaran/core/ediacaran_common.h"
#include "ediacaran/core/expected.h"
#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <string_view>
#include <type_traits>

namespace ediacaran
{
    constexpr bool is_space(char i_char) noexcept { return i_char == ' '; }

    constexpr bool is_digit(char i_char) noexcept { return i_char >= '0' && i_char <= '9'; }

    constexpr bool is_alpha(char i_char) noexcept
    {
        return (i_char >= 'a' && i_char <= 'z') || (i_char >= 'A' && i_char <= 'Z');
    }

    constexpr bool is_alphanum(char i_char) noexcept
    {
        return (i_char >= 'a' && i_char <= 'z') || (i_char >= 'A' && i_char <= 'Z') ||
               (i_char >= '0' && i_char <= '9');
    }

    enum class[[nodiscard]] parse_error{unexpected_char,
                                        unexpected_token,
                                        missing_expected_chars,
                                        mismatching_value,
                                        overflow,
                                        tailing_chars,
                                        out_of_memory,
                                        internal_limit,
                                        unknown_error,
                                        unsupported,
                                        not_found};

    /** Class used to convert a sequence of chars to typed values. char_reader is a
        non-owning view of a null-terminated string of characters.
        While values are parsed or accepted, the reader advances in the string.
        Note: the char after the end mujst be readable until a null char is present. In other
        word even if the view is a part of the buffer, the whole buffer must be null-terminated
        */
    class char_reader
    {
      public:
        constexpr explicit char_reader(const string_view & i_source) noexcept : m_source(i_source)
        {
        }

        constexpr char_reader(const char_reader &) noexcept = default;

        constexpr char_reader & operator=(const char_reader &) noexcept = default;

        constexpr const char * next_chars() const noexcept { return m_source.data(); }

        constexpr size_t remaining_chars() const noexcept { return m_source.size(); }

        constexpr void skip(size_t i_size) noexcept { m_source.remove_prefix(i_size); }

      private:
        string_view m_source;
    };

    // trait has_parse
    template <typename, typename = std::void_t<>> struct has_parse : std::false_type
    {
    };
    template <typename TYPE>
    struct has_parse<
      TYPE,
      std::void_t<decltype(parse(std::declval<TYPE &>(), std::declval<char_reader &>()))>>
        : std::true_type
    {
    };
    template <typename TYPE> using has_parse_t          = typename has_parse<TYPE>::type;
    template <typename TYPE> constexpr bool has_parse_v = has_parse<TYPE>::value;

    // trait has_accept
    template <typename, typename = std::void_t<>> struct has_accept : std::false_type
    {
    };
    template <typename TYPE>
    struct has_accept<
      TYPE,
      std::void_t<decltype(accept(std::declval<const TYPE>(), std::declval<char_reader &>()))>>
        : std::true_type
    {
    };
    template <typename TYPE> using has_accept_t          = typename has_accept<TYPE>::type;
    template <typename TYPE> constexpr bool has_accept_v = has_accept<TYPE>::value;

    // generic parse that returns the value
    template <typename TYPE>
    constexpr expected<TYPE, parse_error> parse(char_reader & i_source) noexcept(
      noexcept(TYPE{}) && noexcept(parse(std::declval<TYPE &>(), std::declval<char_reader &>())))
    {
        TYPE       value{};
        auto const result = parse(value, i_source);
        if (!result)
            return result.error();
        else
            return value;
    }

    // generic accept based on parse
    template <typename TYPE, std::enable_if_t<has_parse_v<TYPE>> * = nullptr>
    expected<void, parse_error>
      accept(const TYPE & i_expected_value, char_reader & i_source) noexcept(
        noexcept(TYPE{}) && noexcept(parse(std::declval<TYPE &>(), std::declval<char_reader &>())))
    {
        TYPE                        actual_value{};
        auto                        source = i_source;
        expected<void, parse_error> result = parse(actual_value, source);
        if (!result)
        {
            return result;
        }
        if (actual_value != i_expected_value)
        {
            return parse_error::mismatching_value;
        }
        i_source = source;
        return {};
    }

    // generic parse from string_view
    template <typename TYPE>
    constexpr expected<TYPE, parse_error> parse(const string_view & i_source) noexcept(
      noexcept(parse<TYPE>(std::declval<char_reader &>())))
    {
        char_reader reader(i_source);
        auto        result = parse<TYPE>(reader);
        if (reader.remaining_chars() != 0)
        {
            return parse_error::tailing_chars;
        }
        return result;
    }

    // generic char_reader >> val, based on parse
    template <typename TYPE>
    constexpr char_reader & operator>>(char_reader & i_source, TYPE & o_dest)
    {
        static_assert(has_parse_v<TYPE>);
        auto const result = parse(o_dest, i_source);
        if (!result)
            throw result.error();
        return i_source;
    }

    // generic char_reader >> const val, based on accept
    template <typename TYPE>
    constexpr char_reader & operator>>(char_reader & i_source, const TYPE & i_expected_value)
    {
        static_assert(has_accept_v<TYPE>);
        auto const result = accept(i_expected_value, i_source);
        if (!result)
            throw result.error();
        return i_source;
    }

    // accept for spaces - they don't have a parse
    constexpr expected<void, parse_error> accept(spaces_t, char_reader & i_source) noexcept
    {
        bool some_chars_skipped = false;
        while (is_space(*i_source.next_chars()))
        {
            i_source.skip(1);
            some_chars_skipped = true;
        }
        if (some_chars_skipped)
            return {};
        else
            return parse_error::unexpected_char;
    }

    // accept for strings - they don't have a parse
    constexpr expected<void, parse_error>
      accept(const string_view & i_expected, char_reader & i_source) noexcept
    {
        for (size_t index = 0; index < i_expected.size(); index++)
        {
            auto const source_char = i_source.next_chars()[index];
            if (source_char == 0 || i_expected[index] != source_char)
                return parse_error::unexpected_char;
        }

        i_source.skip(i_expected.length());
        return {};
    }

    // accept for chars
    constexpr expected<void, parse_error> accept(char i_expected, char_reader & i_source) noexcept
    {
        if (*i_source.next_chars() == i_expected)
        {
            i_source.skip(1);
            return {};
        }
        else
        {
            return parse_error::unexpected_char;
        }
    }

    // parse for chars
    constexpr expected<void, parse_error> parse(char & o_dest, char_reader & i_source) noexcept
    {
        if (i_source.remaining_chars() > 0)
        {
            o_dest = *i_source.next_chars();
            i_source.skip(1);
            return {};
        }
        else
        {
            return parse_error::missing_expected_chars;
        }
    }

    // parse signed integer
    template <
      typename INT_TYPE,
      std::enable_if_t<std::is_integral_v<INT_TYPE> && std::is_signed_v<INT_TYPE>> * = nullptr>
    constexpr expected<void, parse_error> parse(INT_TYPE & o_dest, char_reader & i_source) noexcept
    {
        const char *       curr_digit    = i_source.next_chars();
        const char * const end_of_buffer = curr_digit + i_source.remaining_chars();
        INT_TYPE           result        = 0;

        if (*curr_digit == '-')
        {
            curr_digit++;
            while (curr_digit < end_of_buffer)
            {
                if (*curr_digit >= '0' && *curr_digit <= '9')
                {
                    INT_TYPE const digit      = *curr_digit - '0';
                    INT_TYPE const thereshold = (std::numeric_limits<INT_TYPE>::min() + digit) / 10;
                    if (result < thereshold)
                    {
                        return parse_error::overflow;
                    }
                    result *= 10;
                    result -= digit;
                }
                else
                {
                    break;
                }
                curr_digit++;
            }
        }
        else
        {
            while (curr_digit < end_of_buffer)
            {
                if (*curr_digit >= '0' && *curr_digit <= '9')
                {
                    INT_TYPE const digit      = *curr_digit - '0';
                    INT_TYPE const thereshold = (std::numeric_limits<INT_TYPE>::max() - digit) / 10;
                    if (result > thereshold)
                    {
                        return parse_error::overflow;
                    }
                    result *= 10;
                    result += digit;
                }
                else
                {
                    break;
                }
                curr_digit++;
            }
        }

        const size_t accepted_digits = curr_digit - i_source.next_chars();
        if (accepted_digits == 0)
        {
            return parse_error::missing_expected_chars;
        }
        else
        {
            i_source.skip(accepted_digits);
            o_dest = result;
            return {};
        }
    }

    // parse unsigned integer
    template <
      typename UINT_TYPE,
      std::enable_if_t<std::is_integral_v<UINT_TYPE> && !std::is_signed_v<UINT_TYPE>> * = nullptr>
    constexpr expected<void, parse_error> parse(UINT_TYPE & o_dest, char_reader & i_source) noexcept
    {
        const char *       curr_digit    = i_source.next_chars();
        const char * const end_of_buffer = curr_digit + i_source.remaining_chars();
        UINT_TYPE          result        = 0;

        while (curr_digit < end_of_buffer)
        {
            if (*curr_digit >= '0' && *curr_digit <= '9')
            {
                UINT_TYPE const digit      = *curr_digit - '0';
                UINT_TYPE const thereshold = (std::numeric_limits<UINT_TYPE>::max() - digit) / 10;
                if (result > thereshold)
                {
                    return parse_error::overflow;
                }
                result *= 10;
                result += digit;
            }
            else
            {
                break;
            }
            curr_digit++;
        }

        size_t const accepted_digits = curr_digit - i_source.next_chars();
        if (accepted_digits == 0)
        {
            return parse_error::missing_expected_chars;
        }
        else
        {
            i_source.skip(accepted_digits);
            o_dest = result;
            return {};
        }
    }

    constexpr expected<void, parse_error> parse(bool & o_dest, char_reader & i_source) noexcept
    {
        if (accept("true", i_source))
        {
            o_dest = true;
            return {};
        }

        if (accept("false", i_source))
        {
            o_dest = false;
            return {};
        }

        return parse_error::unexpected_token;
    }

    constexpr string_view try_parse_identifier(char_reader & i_source) noexcept
    {
        auto next_char = i_source.next_chars();
        if (is_alpha(*next_char) || *next_char == '_')
        {
            while (is_alphanum(*next_char) || *next_char == '_')
                next_char++;
        }
        auto const first_char = i_source.next_chars();
        EDIACARAN_INTERNAL_ASSERT(next_char >= first_char);
        auto const length = static_cast<size_t>(next_char - first_char);
        i_source.skip(length);
        return {first_char, length};
    }

    expected<void, parse_error> parse(float & o_dest, char_reader & i_source) noexcept;
    expected<void, parse_error> parse(double & o_dest, char_reader & i_source) noexcept;
    expected<void, parse_error> parse(long double & o_dest, char_reader & i_source) noexcept;
}
