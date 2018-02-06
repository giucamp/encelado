
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
    using page_address                          = uint64_t;
    constexpr page_address invalid_page_address = std::numeric_limits<page_address>::max();

    class storage_device
    {
      public:
        enum class error
        {

        };

        enum class access_flags
        {

        };

        virtual expected<void *, error>
          begin_access_page(page_address i_address, access_flags i_flags) noexcept = 0;

        virtual void end_access_page(void * i_page) noexcept = 0;

        virtual ~storage_device() = default;
    };


} // namespace cambrian
