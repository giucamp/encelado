
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "cambrian/cambrian_common.h"
#include "ediacaran/reflection/type.h"
#include <vector>

namespace cambrian
{
    using TypeId = uint64_t;

    class TypeRegistry
    {
      public:
        TypeRegistry();

        TypeId add_type(const type * i_type);

      private:
        std::vector<const type *> m_types;
    };
} // namespace cambrian
