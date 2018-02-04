
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.

#pragma once
#include "CambrianCommon.h"
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
}
