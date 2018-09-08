
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "cambrian/data/directory.h"
#include "ediacaran/core/string_utils.h"

namespace cambrian
{
    /*obj_ref::obj_ref(storage_device * i_device, const string_view & i_path) : m_device(i_device)
    {
        auto const device_info = m_device->get_info();
        auto const slot_count  = device_info.m_page_size / sizeof(page_address);

        auto page = device_info.m_root_page;

        for (auto & token : split(i_path, '/'))
        {
            auto mapping =
              i_device->map_page(page, storage_device::access_flags::read_write).value();
            m_mapped_pages.push_back(std::move(mapping));
        }
    }*/

    directory_iterator::directory_iterator(storage_device * i_device, const string_view & i_path)
        : m_device(i_device), m_path(i_path)
    {
    }

    /*void directory_iterator::lookup(const string_view & i_path)
    {
        auto const device_info = m_device->get_info();
        auto const slot_count  = device_info.m_page_size / sizeof(page_address);

        void * current_content =
          m_device
            ->begin_access_page(device_info.m_root_page, storage_device::access_flags::read_write)
            .value();

        for (auto & token : split(i_path, '/'))
        {
            auto const slot_index = hash(token) % slot_count;
            auto const next_page  = static_cast<page_address *>(current_content)[slot_index];
            auto       is_dictionary_page = (next_page & dictionary_page_mask) != 0;
            void *     next_content =
              m_device
                ->begin_access_page(
                  next_page & ~dictionary_page_mask, storage_device::access_flags::read_write)
                .value();
            ;
            if (is_dictionary_page)
            {
            }
            m_device->end_access_page(current_content);
            current_content = next_content;
        }

        auto token_it = split(i_path, '/').begin();
        bool is_dictionary_page;
        do
        {

        } while (token_it != end_marker);
    }*/

} // namespace cambrian
