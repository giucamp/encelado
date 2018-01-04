
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2016-2017.

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
}
