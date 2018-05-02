
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "cambrian/cambrian_common.h"
#include "ediacaran/reflection/class_type.h"
#include <memory>
#include <unordered_map>

namespace cambrian
{
    using type_id = uint32_t;

    class type_registry
    {
      public:
        struct type_data
        {
            const type &  m_native_type;
            type_id const m_id;
            const type &  m_passive_type;
        };

        type_data get_type_data(const type & i_source_type);

      private:
        struct PassiveClassData;
        struct PassiveClass;

      private:
        std::unordered_map<const class_type *, std::unique_ptr<const PassiveClass>> m_classes;
        type_id m_next_type_id = 0;
    };

} // namespace cambrian
