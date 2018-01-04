
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2016-2017.


//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2016-2017.


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
