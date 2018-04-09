
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "cambrian/cambrian_common.h"
#include "cambrian/storage/storage_device.h"
#include <stdio.h>

namespace cambrian
{
    class file_device final : public storage_device
    {
      public:
        file_device(const string_view & i_file_name);

        ~file_device();

        expected<mapped_page, error> allocate_page() noexcept override;

        void deallocate_page(page_address i_address) noexcept override;

        expected<mapped_page, error>
          map_page(page_address i_address, access_flags i_flags) noexcept override;

        void unmap_page(const mapped_page & i_page) noexcept override;

      private:
        FILE * m_file;
    };


} // namespace cambrian
