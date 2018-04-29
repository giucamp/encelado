
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "cambrian/cambrian_common.h"
#include "ediacaran/reflection/class_type.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace cambrian
{
    class type_registry
    {
      public:
        const type & get_passive_type(const type & i_source_type);

      private:
        struct PassiveClassData;
        struct PassiveClass;

      private:
        std::unordered_map<const class_type *, std::unique_ptr<const PassiveClass>> m_classes;
    };

} // namespace cambrian
