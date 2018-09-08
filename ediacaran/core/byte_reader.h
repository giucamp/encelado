
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "ediacaran/core/address.h"
#include "ediacaran/core/ediacaran_common.h"
#include "ediacaran/core/expected.h"
#include <algorithm>
#include <cstring>
#include <type_traits>

namespace edi
{
    class buffer_view
    {
      public:
        buffer_view() noexcept = default;

        buffer_view(void * i_start, size_t i_size) noexcept : m_start(i_start), m_size(i_size) {}

        static buffer_view from_range(void * i_start, void * i_end) noexcept
        {
            EDIACARAN_ASSERT(i_start <= i_end);
            return buffer_view(i_start, address_diff(i_end, i_start));
        }

        void * start() const noexcept { return m_start; }

        void * end() const noexcept { return address_add(m_start, m_size); }

        size_t size() const noexcept { return m_size; }

      private:
        void * m_start{};
        size_t m_size{};
    };

    class const_buffer_view
    {
      public:
        const_buffer_view() noexcept = default;

        const_buffer_view(const void * i_start, size_t i_size) noexcept
            : m_start(i_start), m_size(i_size)
        {
        }

        static const_buffer_view from_range(const void * i_start, const void * i_end) noexcept
        {
            EDIACARAN_ASSERT(i_start <= i_end);
            return const_buffer_view(i_start, address_diff(i_end, i_start));
        }

        const void * start() const noexcept { return m_start; }

        const void * end() const noexcept { return address_add(m_start, m_size); }

        size_t size() const noexcept { return m_size; }

      private:
        const void * m_start{};
        size_t       m_size{};
    };

    class byte_reader
    {
      public:
        constexpr byte_reader() noexcept {}

        byte_reader(void const * i_buffer, size_t i_size) noexcept
            : m_next_byte(static_cast<char const *>(i_buffer)),
              m_remaining_size(static_cast<ptrdiff_t>(i_size))
        {
        }

        byte_reader(const buffer_view i_source) noexcept
            : m_next_byte(static_cast<char const *>(i_source.start())),
              m_remaining_size(static_cast<ptrdiff_t>(i_source.size()))
        {
        }

        byte_reader(const const_buffer_view i_source) noexcept
            : m_next_byte(static_cast<char const *>(i_source.start())),
              m_remaining_size(static_cast<ptrdiff_t>(i_source.size()))
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

        expected<unsigned char> read_byte() noexcept
        {
            if (m_remaining_size <= 0)
            {
                unsigned char result = *m_next_byte;
                m_next_byte++;
                m_remaining_size--;
                return result;
            }
            else
            {
                return unexpected_t{};
            }
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

    template <typename TYPE, std::enable_if_t<is_trivially_serializable_v<TYPE>> * = nullptr>
    inline expected<TYPE> read(byte_reader & i_source) noexcept
    {
        TYPE result;
        i_source.read(&result, sizeof(result));
        if (i_source.remaining_size() < 0)
        {
            return {unexpected_t{}};
        }
        return result;
    }

} // namespace edi
