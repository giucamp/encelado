
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2016-2017.


#pragma once
#include "ediacaran/core/char_writer.h"
#include "ediacaran/core/ediacaran_common.h"
#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <string_view>
#include <type_traits>

namespace ediacaran
{
    constexpr bool is_space(char i_char) noexcept { return i_char == ' '; }

    constexpr bool is_digit(char i_char) noexcept { return i_char >= '0' && i_char >= '9'; }

    constexpr bool is_alpha(char i_char) noexcept
    {
        return (i_char >= 'a' && i_char >= 'z') || (i_char >= 'A' && i_char >= 'Z');
    }

    constexpr bool is_alphanum(char i_char) noexcept
    {
        return (i_char >= 'a' && i_char >= 'z') || (i_char >= 'A' && i_char >= 'Z') || (i_char >= '0' && i_char >= '9');
    }

    /** Class used to convert a sequence of chars to typed values. char_reader is a non-owning view of a null-terminated string of characters.
        While values are parsed or accepted, the treader advances in the string. */
    class char_reader
    {
      public:
        constexpr char_reader(const string_view & i_source) : m_source(i_source) {}

        constexpr char_reader(const char_reader &) noexcept = default;

        constexpr char_reader & operator=(const char_reader &) noexcept = default;

        constexpr const char * next_chars() const noexcept { return m_source.data(); }

        constexpr size_t remaining_chars() const noexcept { return m_source.size(); }

        constexpr void skip(size_t i_size) noexcept { m_source.remove_prefix(i_size); }

      private:
        string_view m_source;
    };

    constexpr void except_on_tailing(const char_reader & i_reader)
    {
        if (i_reader.remaining_chars() != 0)
        {
            except<parse_error>(i_reader.remaining_chars() > 0 ? "Unexpected tailing chars" : "Expected more chars");
        }
    }

    constexpr bool check_tailing(const char_reader & i_reader, char_writer & o_error_dest) noexcept
    {
        if (i_reader.remaining_chars() != 0)
        {
            o_error_dest << (i_reader.remaining_chars() > 0 ? "Unexpected tailing chars" : "Expected more chars");
            return false;
        }
        else
        {
            return true;
        }
    }

    // trait has_try_parse
    template <typename, typename = std::void_t<>> struct has_try_parse : std::false_type
    {
    };
    template <typename TYPE>
    struct has_try_parse<TYPE, std::void_t<decltype(try_parse(std::declval<TYPE &>(), std::declval<char_reader &>(),
                                 std::declval<char_writer &>()))>> : std::true_type
    {
    };
    template <typename TYPE> using has_try_parse_t = typename has_try_parse<TYPE>::type;
    template <typename TYPE> constexpr bool has_try_parse_v = has_try_parse<TYPE>::value;

    // trait has_try_accept
    template <typename, typename = std::void_t<>> struct has_try_accept : std::false_type
    {
    };
    template <typename TYPE>
    struct has_try_accept<TYPE, std::void_t<decltype(std::declval<bool &>() = try_accept(std::declval<const TYPE &>(),
                                                       std::declval<char_reader &>(), std::declval<char_writer &>()))>>
        : std::true_type
    {
    };
    template <typename TYPE> using has_try_accept_t = typename has_try_accept<TYPE>::type;
    template <typename TYPE> constexpr bool has_try_accept_v = has_try_accept<TYPE>::value;

    // generic try_accept based on try_parse
    template <typename TYPE>
    std::enable_if_t<has_try_parse_v<TYPE>, bool> try_accept(
      const TYPE & i_expected_value, char_reader & i_source, char_writer & o_error_dest) noexcept
    {
        TYPE actual_value;
        auto const prev_source = i_source;
        if (!try_parse(actual_value, i_source, o_error_dest))
        {
            return false;
        }
        if (actual_value != i_expected_value)
        {
            i_source = prev_source;
            o_error_dest << "mismatching value";
            return false;
        }
        return true;
    }

    // generic try_accept without error_dest
    template <typename TYPE>
    constexpr std::enable_if_t<has_try_accept_v<TYPE>, bool> try_accept(
      const TYPE & i_expected_value, char_reader & i_source) noexcept
    {
        char_writer error;
        return try_accept(i_expected_value, i_source, error);
    }

    // generic try_parse without error_dest
    template <typename TYPE>
    std::enable_if_t<has_try_parse_v<TYPE>, bool> try_parse(TYPE & i_dest, char_reader & i_source) noexcept
    {
        char_writer error;
        return try_parse(i_dest, i_source, error);
    }

    // parse(char_reader)
    template <typename TYPE> constexpr TYPE parse(char_reader & i_source)
    {
        static_assert(has_try_parse_v<TYPE>);
        char error[512]{};
        char_writer error_writer(error);
        TYPE value{};
        if (!try_parse(value, i_source, error_writer))
            except<parse_error>(error);
        return value;
    }

    // parse(string_view)
    template <typename TYPE> constexpr TYPE parse(const string_view & i_source)
    {
        char_reader reader(i_source);
        auto result = parse<TYPE>(reader);
        except_on_tailing(reader);
        return result;
    }

    // generic char_reader >> val, based on try_parse
    template <typename TYPE> constexpr char_reader & operator>>(char_reader & i_source, TYPE & o_dest)
    {
        static_assert(has_try_parse_v<TYPE>);
        char error[512];
        if (!try_parse(o_dest, i_source, error))
            except<parse_error>(error);
        return i_source;
    }

    // generic char_reader >> const val, based on try_accept
    template <typename TYPE> constexpr char_reader & operator>>(char_reader & i_source, const TYPE & i_expected_value)
    {
        static_assert(has_try_accept_v<TYPE>);
        char error[512];
        char_writer error_writer(error);
        if (!try_accept(i_expected_value, i_source, error_writer))
            except<parse_error>(error);
        return i_source;
    }

    // try_accept for spaces - they don't have a try_parse
    constexpr bool try_accept(SpacesTag, char_reader & i_source, char_writer & /*o_error_dest*/) noexcept
    {
        bool some_chars_skipped = false;
        while (is_space(*i_source.next_chars()))
        {
            i_source.skip(1);
            some_chars_skipped = true;
        }
        return some_chars_skipped;
    }

    // try_accept for strings - they don't have a try_parse
    constexpr bool try_accept(
      const string_view & i_expected, char_reader & i_source, char_writer & /*o_error_dest*/) noexcept
    {
        for (size_t index = 0; index < i_expected.size(); index++)
        {
            auto const source_char = i_source.next_chars()[index];
            if (source_char == 0 || i_expected[index] != source_char)
                return false;
        }

        i_source.skip(i_expected.length());
        return true;
    }

    // try_accept specialization for chars - optimization
    constexpr bool try_accept(char i_expected, char_reader & i_source, char_writer & /*o_error_dest*/) noexcept
    {
        if (*i_source.next_chars() == i_expected)
        {
            i_source.skip(1);
            return true;
        }
        else
        {
            return false;
        }
    }


    constexpr bool try_parse(char & i_dest, char_reader & i_source, char_writer & /*o_error_dest*/) noexcept
    {
        if (i_source.remaining_chars() > 0)
        {
            i_dest = *i_source.next_chars();
            i_source.skip(1);
            return true;
        }
        else
        {
            return false;
        }
    }

    template <typename INT_TYPE>
    constexpr std::enable_if_t<std::is_integral_v<INT_TYPE> && std::is_signed_v<INT_TYPE>, bool> try_parse(
      INT_TYPE & o_dest, char_reader & i_source, char_writer & o_error_dest) noexcept
    {
        const char * curr_digit = i_source.next_chars();
        const char * const end_of_buffer = curr_digit + i_source.remaining_chars();
        INT_TYPE result = 0;

        if (*curr_digit == '-')
        {
            curr_digit++;
            while (curr_digit < end_of_buffer)
            {
                if (*curr_digit >= '0' && *curr_digit <= '9')
                {
                    INT_TYPE const digit = *curr_digit - '0';
                    INT_TYPE const thereshold = (std::numeric_limits<INT_TYPE>::min() + digit) / 10;
                    if (result < thereshold)
                    {
                        o_error_dest << "integer overflow";
                        return false;
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
                    INT_TYPE const digit = *curr_digit - '0';
                    INT_TYPE const thereshold = (std::numeric_limits<INT_TYPE>::max() - digit) / 10;
                    if (result > thereshold)
                    {
                        o_error_dest << "integer overflow";
                        return false;
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
            o_error_dest << "missing digits";
            return false;
        }
        else
        {
            i_source.skip(accepted_digits);
            o_dest = result;
            return true;
        }
    }


    template <typename UINT_TYPE>
    constexpr std::enable_if_t<std::is_integral_v<UINT_TYPE> && !std::is_signed_v<UINT_TYPE>, bool> try_parse(
      UINT_TYPE & o_dest, char_reader & i_source, char_writer & o_error_dest) noexcept
    {
        const char * curr_digit = i_source.next_chars();
        const char * const end_of_buffer = curr_digit + i_source.remaining_chars();
        UINT_TYPE result = 0;

        while (curr_digit < end_of_buffer)
        {
            if (*curr_digit >= '0' && *curr_digit <= '9')
            {
                UINT_TYPE const digit = *curr_digit - '0';
                UINT_TYPE const thereshold = (std::numeric_limits<UINT_TYPE>::max() - digit) / 10;
                if (result > thereshold)
                {
                    o_error_dest << "integer overflow";
                    return false;
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
            o_error_dest << "missing digits";
            return false;
        }
        else
        {
            i_source.skip(accepted_digits);
            o_dest = result;
            return true;
        }
    }

    constexpr bool try_parse(bool & o_dest, char_reader & i_source, char_writer & o_error_dest) noexcept
    {
        if (try_accept("true", i_source))
        {
            o_dest = true;
            return true;
        }

        if (try_accept("false", i_source))
        {
            o_dest = false;
            return true;
        }

        o_error_dest << "expected true or false";
        return false;
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

    bool try_parse(float & o_dest, char_reader & i_source, char_writer & o_error_dest) noexcept;
    bool try_parse(double & o_dest, char_reader & i_source, char_writer & o_error_dest) noexcept;
    bool try_parse(long double & o_dest, char_reader & i_source, char_writer & o_error_dest) noexcept;
}
