
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "TypeRegistry.h"
#include "ediacaran/reflection/reflection.h"

namespace cambrian
{
    TypeRegistry::TypeRegistry()
    {
        add_type(&get_type<int8_t>());
        add_type(&get_type<int16_t>());
        add_type(&get_type<int32_t>());
        add_type(&get_type<int64_t>());
        add_type(&get_type<uint8_t>());
        add_type(&get_type<uint16_t>());
        add_type(&get_type<uint32_t>());
        add_type(&get_type<uint64_t>());
    }

    TypeId TypeRegistry::add_type(const type * i_type)
    {
        auto Id = m_types.size();
        m_types.push_back(i_type);
        return Id;
    }
} // namespace cambrian
