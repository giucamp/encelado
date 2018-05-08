
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "ediacaran/core/address.h"
#include "ediacaran/core/ediacaran_common.h"
#include "ediacaran/core/expected.h"
#include <algorithm>
#include <type_traits>
#include <cstring>

namespace edi
{

    class byte_reader
    {
      public:
        constexpr byte_reader() noexcept {}

        byte_reader(void const * i_buffer, size_t i_size) noexcept
            : m_next_byte(static_cast<char const *>(i_buffer)),
              m_remaining_size(static_cast<ptrdiff_t>(i_size))
        {
        }

        constexpr ptrdiff_t remaining_size() const noexcept { return m_remaining_size; }

        void read(void * i_dest, size_t i_size) noexcept
        {
            auto const count_to_read = std::min(m_remaining_size, static_cast<ptrdiff_t>(i_size));
            memcpy(i_dest, m_next_byte, count_to_read);
            m_next_byte += count_to_read;
            m_remaining_size -= static_cast<ptrdiff_t>(i_size);
        }

      private:
        const char * m_next_byte{};
        ptrdiff_t    m_remaining_size{};
    };

    template <typename TYPE, std::enable_if_t<is_trivially_serializable_v<TYPE>> * = nullptr>
    inline byte_reader & operator>>(byte_reader & i_source, TYPE & i_dest_object)
    {
        i_source.read(&i_dest_object, sizeof(i_dest_object));
        if (i_source.remaining_size() < 0)
            except<std::runtime_error>("byte_reader is out of space");
        return i_source;
    }

    enum class read_result
    {
        ok,
        out_of_space,
    };

    template <typename TYPE, std::enable_if_t<is_trivially_serializable_v<TYPE>> * = nullptr>
    inline expected<TYPE, read_result> read(byte_reader & i_source) noexcept
    {
        TYPE result;
        i_source.read(&result, sizeof(result));
        if (i_source.remaining_size() < 0)
        {
            return read_result::out_of_space;
        }
        return result;
    }

} // namespace edi
