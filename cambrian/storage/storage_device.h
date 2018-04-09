
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "cambrian/cambrian_common.h"
#include "ediacaran/core/expected.h"
#include <limits>

namespace cambrian
{
    using page_address                            = uint64_t;
    using page_size                               = uint32_t;
    constexpr bit_index    page_address_user_bits = 2;
    constexpr page_address invalid_page_address =
      ~uint_mask_rev<page_address>(0, page_address_user_bits);


    class mapped_page
    {
      public:
        mapped_page() noexcept = default;

        mapped_page(page_address i_storage_address, void * i_mem_address) noexcept
            : m_storage_address(i_storage_address), m_mem_address(i_mem_address)
        {
        }

        mapped_page(mapped_page && i_source) noexcept
            : m_storage_address(i_source.m_storage_address), m_mem_address(i_source.m_mem_address)
        {
            i_source.m_storage_address = {};
            i_source.m_mem_address     = {};
        }

        mapped_page & operator=(mapped_page && i_source) noexcept
        {
            m_storage_address          = i_source.m_storage_address;
            m_mem_address              = i_source.m_mem_address;
            i_source.m_storage_address = {};
            i_source.m_mem_address     = {};
            return *this;
        }

        ~mapped_page() { CAMBRIAN_ASSERT(m_storage_address == 0 && m_mem_address == nullptr); }

        mapped_page(const mapped_page &) = delete;
        mapped_page & operator=(const mapped_page &) = delete;

      private:
        page_address m_storage_address{};
        void *       m_mem_address{};
    };

    class storage_device
    {
      public:
        storage_device() noexcept              = default;
        storage_device(const storage_device &) = delete;
        storage_device & operator=(const storage_device &) = delete;

        enum class error
        {

        };

        enum class access_flags
        {
            read       = 1 << 0,
            write      = 1 << 1,
            read_write = read | write
        };

        struct info
        {
            page_size    m_page_size;
            page_address m_root_page;
        };

        virtual info get_info() noexcept = 0;

        virtual expected<mapped_page, error> allocate_page() noexcept = 0;

        virtual void deallocate_page(page_address i_address) noexcept = 0;

        virtual expected<mapped_page, error>
          map_page(page_address i_address, access_flags i_flags) noexcept = 0;

        virtual void unmap_page(const mapped_page & i_page) noexcept = 0;

        virtual ~storage_device() = default;
    };

    constexpr storage_device::access_flags
      operator|(storage_device::access_flags i_first, storage_device::access_flags i_second)
    {
        return static_cast<storage_device::access_flags>(
          static_cast<std::underlying_type_t<storage_device::access_flags>>(i_first) |
          static_cast<std::underlying_type_t<storage_device::access_flags>>(i_second));
    }

    constexpr storage_device::access_flags
      operator&(storage_device::access_flags i_first, storage_device::access_flags i_second)
    {
        return static_cast<storage_device::access_flags>(
          static_cast<std::underlying_type_t<storage_device::access_flags>>(i_first) &
          static_cast<std::underlying_type_t<storage_device::access_flags>>(i_second));
    }

} // namespace cambrian
