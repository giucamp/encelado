
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.

#include "ediacaran/core/char_reader.h"
#include "ediacaran/core/char_writer.h"
#include "ediacaran/reflection/namespace.h"
#include <ediacaran/reflection/reflection.h>

namespace ediacaran
{
    expected<void, parse_error> parse(qualified_type_ptr & o_dest, char_reader & i_source) noexcept
    {
        size_t       constness_word = 0, volatileness_word = 0;
        const type * final_type = nullptr;
        size_t       indirection_levels =
          0; // this variable is not the index of the current IL, but the number of IL's so far
        auto source = i_source;

        for (;;)
        {
            (void)accept(spaces, source);

            if (accept("const", source))
            {
                constness_word |= 1;
            }
            else if (accept("volatile", source))
            {
                volatileness_word |= 1;
            }
            else if (accept('*', source))
            {
                constness_word <<= 1;
                volatileness_word <<= 1;
                indirection_levels++;
                if (indirection_levels > qualified_type_ptr::s_max_indirection_levels)
                {
                    break;
                }
            }
            else if (accept('&', source))
            {
                (void)accept('&', source);
                constness_word <<= 1;
                constness_word |= 1;
                volatileness_word <<= 1;
                indirection_levels++;
                break;
            }
            else if (
              indirection_levels == 0 &&
              final_type ==
                nullptr) // only in the last indirection level (that is before any *, & or &&
            {
                if (auto res = parse(&final_type, source); !res)
                {
                    return res.error();
                }
            }
            else
            {
                break;
            }
        }

        if (indirection_levels > qualified_type_ptr::s_max_indirection_levels)
        {
            return parse_error::internal_limit;
        }
        else if (final_type == nullptr)
        {
            return parse_error::missing_expected_chars;
            ;
        }

        // commit
        i_source = source;
        o_dest =
          qualified_type_ptr(final_type, indirection_levels, constness_word, volatileness_word);
        return {};
    }
}
