
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "cambrian/cambrian_common.h"
#include "cambrian/storage/storage_device.h"

namespace cambrian
{

    class directory_iterator
    {
      public:
        directory_iterator(storage_device * i_device, const string_view & i_path);

      private:
        void open_page(const string_view & i_path);

      private:
        storage_device * m_device;
        string_view     m_path;
    };


} // namespace cambrian
