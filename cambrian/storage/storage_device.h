
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
    using page_address = uint64_t;
    using page_size    = uint32_t;
    constexpr bit_index page_address_user_bits = 2;
    constexpr page_address invalid_page_address = ~uint_mask_rev<page_address>(0, page_address_user_bits);

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

        struct new_page
        {
            void *       m_memory;
            page_address m_address;
        };

        struct info
        {
            page_size    m_page_size;
            page_address m_root_page;
        };

        virtual info get_info() noexcept = 0;

        virtual expected<new_page, error> allocate_page() noexcept = 0;

        virtual void deallocate_page(page_address i_address) noexcept = 0;

        virtual expected<void *, error>
          begin_access_page(page_address i_address, access_flags i_flags) noexcept = 0;

        virtual void end_access_page(void * i_page) noexcept = 0;

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
