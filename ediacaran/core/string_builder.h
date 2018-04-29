
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <array>
#include <ediacaran/core/char_writer.h>
#include <memory>
#include <utility>
#include <vector>

namespace edi
{
    class string_builder
    {
      public:
        string_builder() noexcept
        {
            m_writer = char_writer(m_inplace_space.data(), m_inplace_space.size());
        }

        string_builder(string_builder && i_source)
            : m_chunks(std::move(i_source.m_chunks)), m_inplace_space(i_source.m_inplace_space),
              m_size(i_source.m_size), m_inplace_size(i_source.m_inplace_size)
        {
            m_writer = char_writer(m_inplace_space.data(), m_inplace_space.size());
        }

        friend void swap(string_builder & i_first, string_builder & i_second) noexcept
        {
            auto const first_offset = i_first.m_writer.next_dest() - i_first.m_inplace_space.data();
            auto const first_remaining_size = i_first.m_writer.remaining_size();

            auto const second_offset =
              i_second.m_writer.next_dest() - i_second.m_inplace_space.data();
            auto const second_remaining_size = i_second.m_writer.remaining_size();

            std::swap(i_first.m_chunks, i_second.m_chunks);
            std::swap(i_first.m_inplace_space, i_second.m_inplace_space);
            std::swap(i_first.m_size, i_second.m_size);
            std::swap(i_first.m_inplace_size, i_second.m_inplace_size);

            i_first.m_writer = char_writer::from_ptr_and_remaining_size(
              i_first.m_inplace_space.data() + second_offset, second_remaining_size);
            i_second.m_writer = char_writer::from_ptr_and_remaining_size(
              i_second.m_inplace_space.data() + first_offset, first_remaining_size);
        }

        string_builder & operator=(string_builder && i_source) noexcept
        {
            swap(*this, i_source);
            return *this;
        }

        size_t size() const noexcept;

        std::string to_string() const;

        std::unique_ptr<char[]> to_unique_chars() const;

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
        void new_chunk();

        struct Chunk
        {
            std::unique_ptr<char[]> m_chars;
            size_t
              m_size; /**< for all the chunks but the last: number of chars written in this chunk
                for the last chunk: capacity of the chunk (number of allocated bytes) */

            Chunk(size_t i_size) : m_chars(new char[i_size]), m_size(i_size) {}
        };

      private:
        char_writer          m_writer;
        std::vector<Chunk>   m_chunks;
        std::array<char, 32> m_inplace_space;
        size_t               m_size         = 0;
        size_t               m_inplace_size = 0;
    };

    template <typename... TYPE> std::string to_string(const TYPE &... i_objects)
    {
        string_builder builder;
        (builder << ... << i_objects);
        return builder.to_string();
    }

} // namespace edi
