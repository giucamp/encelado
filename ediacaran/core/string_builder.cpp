
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ediacaran/core/string_builder.h>

namespace edi
{
    size_t string_builder::size() const noexcept
    {
        if (m_chunks.size() > 0)
        {
            auto const last_written_size =
              (m_chunks.back().m_size - 1) - static_cast<size_t>(m_writer.remaining_size());
            return m_size + last_written_size;
        }
        else
        {
            EDIACARAN_INTERNAL_ASSERT(m_size == 0);
            return (std::extent_v<decltype(m_inplace_space)> - 1) -
                   static_cast<size_t>(m_writer.remaining_size());
        }
    }

    std::string string_builder::to_string() const
    {
        std::string result;
        auto const  string_size = size();
        result.reserve(string_size);

        if (m_chunks.size() > 0)
        {
            result.append(m_inplace_space, m_inplace_size);
            for (size_t chunk_index = 0; chunk_index < m_chunks.size() - 1; chunk_index++)
            {
                auto const & chunk = m_chunks[chunk_index];
                result.append(chunk.m_chars.get(), chunk.m_size);
            }
            EDIACARAN_INTERNAL_ASSERT(m_writer.remaining_size() >= 0);

            auto const & last_chunk = m_chunks.back();
            auto const   last_written_size =
              (last_chunk.m_size - 1) - static_cast<size_t>(m_writer.remaining_size());
            result.append(last_chunk.m_chars.get(), last_written_size);
        }
        else
        {
            auto const size = (std::extent_v<decltype(m_inplace_space)> - 1) -
                              static_cast<size_t>(m_writer.remaining_size());
            result.append(m_inplace_space, size);
        }
        EDIACARAN_INTERNAL_ASSERT(result.size() == string_size);
        return result;
    }

    EDIACARAN_NO_INLINE void string_builder::new_chunk()
    {
        auto const remaining_size = m_writer.remaining_size();
        EDIACARAN_INTERNAL_ASSERT(remaining_size >= 0);
        size_t prev_chunk_size;
        if (m_chunks.size() == 0)
        {
            prev_chunk_size = std::extent_v<decltype(m_inplace_space)>;
            m_inplace_size  = (prev_chunk_size - 1) - static_cast<size_t>(remaining_size);
            EDIACARAN_INTERNAL_ASSERT(m_size == 0);
            m_size = m_inplace_size;
        }
        else
        {
            auto & last_chunk = m_chunks.back();
            prev_chunk_size   = last_chunk.m_size;
            last_chunk.m_size = (prev_chunk_size - 1) - static_cast<size_t>(remaining_size);
            m_size += last_chunk.m_size;
        }
        auto   new_chunk_size = prev_chunk_size * 2;
        auto & new_chunk      = m_chunks.emplace_back(new_chunk_size);
        m_writer              = char_writer(new_chunk.m_chars.get(), new_chunk_size);
    }

} // namespace edi
