
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <ediacaran/core/char_writer.h>
#include <memory>
#include <vector>

namespace edi
{
    class string_builder
    {
      public:
        string_builder() noexcept { m_writer = char_writer(m_inplace_space); }

        size_t size() const noexcept;

        std::string to_string() const;

        template <typename WRITER_FUNC> void custom_write(const WRITER_FUNC & i_function)
        {
            for (;;)
            {
                auto writer = m_writer;
                i_function(writer);
                if (writer.remaining_size() >= 0)
                {
                    m_writer = writer;
                    break;
                }
                new_chunk();
            }
        }

        template <typename TYPE> string_builder & operator<<(const TYPE & i_value)
        {
            static_assert(
              is_stringizable_v<TYPE>,
              "Overloaded operator required: "
              "char_writer & operator << "
              "(const TYPE &)");
            for (;;)
            {
                auto writer = m_writer;
                writer << i_value;
                if (writer.remaining_size() >= 0)
                {
                    m_writer = writer;
                    return *this;
                }
                new_chunk();
            }
        }

      private:
        EDIACARAN_NO_INLINE void new_chunk();

        struct Chunk
        {
            std::unique_ptr<char[]> m_chars;
            size_t
              m_size; /**< for all the chunks but the last: number of chars written in this chunk
                for the last chunk: capacity of the chunk (number of allocated bytes) */

            Chunk(size_t i_size) : m_chars(new char[i_size]), m_size(i_size) {}
        };

      private:
        char_writer        m_writer;
        std::vector<Chunk> m_chunks;
        char               m_inplace_space[32];
        size_t             m_size         = 0;
        size_t             m_inplace_size = 0;
    };

    template <typename... TYPE> std::string to_string(const TYPE &... i_objects)
    {
        string_builder builder;
        (builder << ... << i_objects);
        return builder.to_string();
    }

} // namespace edi
