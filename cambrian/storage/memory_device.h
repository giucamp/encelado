
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "cambrian/cambrian_common.h"
#include "cambrian/storage/storage_device.h"

namespace cambrian
{
    class memory_device final : public storage_device
    {
      public:
        static_assert(sizeof(page_address) >= sizeof(void *));

        expected<mapped_page, error>
          map_page(page_address i_address, access_flags /*i_flags*/) noexcept override
        {
            return mapped_page(i_address, reinterpret_cast<void *>(i_address));
        }

        void unmap_page(const mapped_page & /*i_page*/) noexcept override {}
    };


} // namespace cambrian
