
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2016-2017.


#include "ediacaran/reflection/qualified_type_ptr.h"
#include "ediacaran/core/char_reader.h"
#include "ediacaran/core/char_writer.h"
#include "ediacaran/reflection/namespace.h"

namespace ediacaran
{
    char_writer & operator<<(char_writer & o_dest, const qualified_type_ptr & i_source) noexcept
    {
        if (!i_source.is_empty())
        {
            o_dest << i_source.final_type()->name();

            uintptr_t level = i_source.indirection_levels();
            for (;;)
            {
                if (i_source.is_const(level))
                {
                    o_dest << " const";
                }

                if (i_source.is_volatile(level))
                {
                    o_dest << " volatile";
                }

                if (level == 0)
                    break;

                level--;
                o_dest << " *";
            }
        }
        return o_dest;
    }

    bool try_parse(qualified_type_ptr & o_dest, char_reader & i_source, char_writer & o_error_dest) noexcept
    {
        size_t       constness_word = 0, volatileness_word = 0;
        const type * final_type = nullptr;
        size_t       indirection_levels =
          0; // this variable is not the index of the current IL, but the number of IL's so far
        auto source = i_source;

        for (;;)
        {
            try_accept(spaces, source);

            if (try_accept("const", source))
            {
                constness_word |= 1;
            }
            else if (try_accept("volatile", source))
            {
                volatileness_word |= 1;
            }
            else if (try_accept('*', source))
            {
                constness_word <<= 1;
                volatileness_word <<= 1;
                indirection_levels++;
                if (indirection_levels > qualified_type_ptr::s_max_indirection_levels)
                {
                    break;
                }
            }
            else if (try_accept('&', source))
            {
                try_accept('&', source);
                constness_word <<= 1;
                constness_word |= 1;
                volatileness_word <<= 1;
                indirection_levels++;
                break;
            }
            else if (
              indirection_levels == 0 &&
              final_type == nullptr) // only in the last indirection level (that is before any *, & or &&
            {
                if (!try_parse(&final_type, source, o_error_dest))
                {
                    o_error_dest << "Could not parse the type\n";
                    return false;
                }
            }
            else
            {
                break;
            }
        }

        if (indirection_levels > qualified_type_ptr::s_max_indirection_levels)
        {
            o_error_dest << "Exceeded the maximum number of indirection levels ("
                         << qualified_type_ptr::s_max_indirection_levels << ")";
            return false;
        }
        else if (final_type == nullptr)
        {
            o_error_dest << "Missing final type";
            return false;
        }

        // commit
        i_source = source;
        o_dest   = qualified_type_ptr(final_type, indirection_levels, constness_word, volatileness_word);
        return true;
    }
}
