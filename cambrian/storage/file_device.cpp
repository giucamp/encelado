
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "cambrian/storage/file_device.h"
#include <string>

namespace cambrian
{
    file_device::file_device(const string_view & i_file_name)
    {
        std::string file_name(i_file_name);

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

        // open existing for update
        m_file = fopen(file_name.c_str(), "r+b");
        if (m_file == nullptr)
        {
            // create empty and open for update
            m_file = fopen(file_name.c_str(), "w+b");
        }

#ifdef _MSC_VER
#pragma warning(pop)
#endif

        if (m_file == nullptr)
            throw std::runtime_error("Could not open/create the file " + file_name);
    }

    file_device::~file_device() { fclose(m_file); }

    expected<mapped_page, storage_device::error> file_device::allocate_page() noexcept
    {
        return mapped_page{};
    }

    void file_device::deallocate_page(page_address i_address) noexcept {}

    expected<mapped_page, storage_device::error>
      file_device::map_page(page_address i_address, access_flags i_flags) noexcept
    {
        return mapped_page{};
    }

    void file_device::unmap_page(const mapped_page & i_page) noexcept {}

} // namespace cambrian
