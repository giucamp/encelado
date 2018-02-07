
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "cambrian/data/directory.h"
#include "ediacaran/core/string_utils.h"

namespace cambrian
{
    directory_iterator::directory_iterator(storage_device * i_device, const string_view & i_path)
        : m_device(i_device), m_path(i_path)
    {
    }

    void directory_iterator::open_page(const string_view & i_path)
    {
        auto const device_info = m_device->get_info();
        auto const slot_count  = device_info.m_page_size / sizeof(page_address);

        void * current_page = m_device->begin_access_page(
          device_info.m_root_page, storage_device::access_flags::read_write);

        for (auto const & token : split(i_path, '/'))
        {
            auto const token_hash = hash(token);
            auto const slot_index = token_hash % slot_count;
            auto const next_page  = static_cast<page_address *>(current_page)[slot_index];
        }
    }

} // namespace cambrian
