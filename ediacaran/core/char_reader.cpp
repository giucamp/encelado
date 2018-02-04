
#include "char_reader.h"
#include <cstdio>
#include <inttypes.h>
#include <stdexcept>
#include <string>
#include <system_error>

namespace ediacaran
{
    namespace detail
    {
        namespace
        {
            template <typename FLOAT_TYPE, typename STRTOF>
            expected<void, parse_error>
              parse_float(FLOAT_TYPE & o_dest, char_reader & i_source, STRTOF i_strtof) noexcept
            {
                auto const source = i_source.next_chars();

                char * end_of_number;
                auto   res = i_strtof(source, &end_of_number);

                if (end_of_number != nullptr && end_of_number != source)
                {
                    EDIACARAN_INTERNAL_ASSERT(end_of_number >= source);
                    auto const read_chars = static_cast<size_t>(end_of_number - source);

                    if (read_chars <= i_source.remaining_chars())
                    {
                        o_dest = res;
                        i_source.skip(read_chars);
                        return {};
                    }
                    else
                    {
                        // a number was parsed, but some digit was interpreted past the end of the buffer
                        try
                        {
                            std::string s(source, source + i_source.remaining_chars());
                            res    = i_strtof(source, nullptr);
                            o_dest = res;
                            i_source.skip(read_chars);
                            return {};
                        }
                        catch (const std::bad_alloc &)
                        {
                            return parse_error::out_of_memory;
                        }
                        catch (...)
                        {
                            return parse_error::unknown_error;
                        }
                    }
                }
                return parse_error::unexpected_char;
            }
        }
    }

    expected<void, parse_error> parse(float & o_dest, char_reader & i_source) noexcept
    {
        return detail::parse_float(o_dest, i_source, std::strtof);
    }

    expected<void, parse_error> parse(double & o_dest, char_reader & i_source) noexcept
    {
        return detail::parse_float(o_dest, i_source, std::strtod);
    }

    expected<void, parse_error> parse(long double & o_dest, char_reader & i_source) noexcept
    {
        return detail::parse_float(o_dest, i_source, std::strtold);
    }
}