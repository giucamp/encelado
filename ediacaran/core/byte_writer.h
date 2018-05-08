
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "ediacaran/core/address.h"
#include "ediacaran/core/ediacaran_common.h"
#include <algorithm>
#include <type_traits>
#include <cstring>

namespace edi
{

    class byte_writer
    {
      public:
        byte_writer() noexcept {}

        byte_writer(void * i_buffer, size_t i_size) noexcept
            : m_next_byte(static_cast<char *>(i_buffer)),
              m_remaining_size(static_cast<ptrdiff_t>(i_size))
        {
        }

        constexpr ptrdiff_t remaining_size() const noexcept { return m_remaining_size; }

        void write(void const * i_source, size_t i_size) noexcept
        {
            auto const count_to_write = std::min(m_remaining_size, static_cast<ptrdiff_t>(i_size));
            memcpy(m_next_byte, i_source, count_to_write);
            m_next_byte += count_to_write;
            m_remaining_size -= static_cast<ptrdiff_t>(i_size);
        }

        bool write_all_or_none(void const * i_source, size_t i_size) noexcept
        {
            if (m_remaining_size < static_cast<ptrdiff_t>(i_size))
                return false;
            memcpy(m_next_byte, i_source, static_cast<ptrdiff_t>(i_size));
            m_next_byte += static_cast<ptrdiff_t>(i_size);
            m_remaining_size -= static_cast<ptrdiff_t>(i_size);
            return true;
        }

        void write_unchecked(void const * i_source, size_t i_size) noexcept
        {
            EDIACARAN_ASSERT(m_remaining_size < static_cast<ptrdiff_t>(i_size));
            memcpy(m_next_byte, i_source, static_cast<ptrdiff_t>(i_size));
            m_next_byte += static_cast<ptrdiff_t>(i_size);
            m_remaining_size -= static_cast<ptrdiff_t>(i_size);
        }

      private:
        char *    m_next_byte{};
        ptrdiff_t m_remaining_size{};
    };

    template <typename TYPE, std::enable_if_t<is_trivially_serializable_v<TYPE>> * = nullptr>
    inline byte_writer & operator<<(byte_writer & i_dest, const TYPE & i_source_object)
    {
        i_dest.write(&i_source_object, sizeof(i_source_object));
        if (i_dest.remaining_size() < 0)
            except<std::runtime_error>("byte_writer is out of space");
        return i_dest;
    }

} // namespace edi
