
#include "char_reader.h"
#include <cstdio>
#include <inttypes.h>
#include <stdexcept>
#include <string>

namespace ediacaran
{
    namespace detail
    {
        template <typename FLOAT_TYPE, typename STRTOF>
        bool try_parse_float(FLOAT_TYPE & o_dest, char_reader & i_source, char_writer &, STRTOF i_strtof) noexcept
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
                    return true;
                }
                else
                {
                    std::string s(source, source + i_source.remaining_chars());
                    res    = i_strtof(source, nullptr);
                    o_dest = res;
                    i_source.skip(read_chars);
                    return true;
                }
            }
            return false;
        }
    }

    bool try_parse(float & o_dest, char_reader & i_source, char_writer & o_error_dest) noexcept
    {
        return detail::try_parse_float(o_dest, i_source, o_error_dest, std::strtof);
    }

    bool try_parse(double & o_dest, char_reader & i_source, char_writer & o_error_dest) noexcept
    {
        return detail::try_parse_float(o_dest, i_source, o_error_dest, std::strtod);
    }

    bool try_parse(long double & o_dest, char_reader & i_source, char_writer & o_error_dest) noexcept
    {
        return detail::try_parse_float(o_dest, i_source, o_error_dest, std::strtold);
    }
}