
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
        for (auto const & token : split(i_path, '/'))
        {
            
        }
    }

} // namespace cambrian
