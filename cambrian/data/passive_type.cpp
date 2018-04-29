
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "cambrian/data/passive_type.h"
#include <algorithm>

namespace cambrian
{
    namespace detail
    {
        void passive_class_data::add_properties(
          type_registry * i_type_registry, const array_view<const property> & i_properties)
        {
            for (auto const & prop : i_properties)
            {
                auto const raw_type = prop.qualified_type().final_type();

                auto const prop_passive_type = i_type_registry->get_passive_type(*raw_type);

                auto const prop_name = m_property_names.emplace_back(prop.name());

                m_properties.emplace_back(
                  prop_name.c_str(), qualified_type_ptr(prop_passive_type), m_size);

                m_size += prop_passive_type->size();
            }
        }

        passive_class_data::passive_class_data(
          type_registry * i_type_registry, const class_type & i_source_class)
        {
            m_name = i_source_class.name();

            // count the properties and reserve the space
            size_t property_count = i_source_class.properties().size();
            for (auto const base : i_source_class.bases())
            {
                property_count += base.get_class().properties().size();
            }
            m_properties.reserve(property_count);
            m_property_names.reserve(property_count);


            m_base_classes.reserve(i_source_class.bases().size());

            // add the properties of this class and of the base classes
            add_properties(i_type_registry, i_source_class.properties());
            for (auto const base : i_source_class.bases())
            {
                auto const passive_base = i_type_registry->get_passive_type(base.get_class());
                CAMBRIAN_ASSERT(passive_base->is_class());

                m_base_classes.emplace_back(
                  *static_cast<const class_type *>(passive_base), nullptr);
                add_properties(i_type_registry, base.get_class().properties());
            }
        }

    } // namespace detail

    passive_class::passive_class(type_registry * i_type_registry, const class_type & i_source_class)
        : detail::passive_class_data(i_type_registry, i_source_class),
          class_type(
            detail::passive_class_data::m_name.c_str(),
            detail::passive_class_data::m_size,
            1,
            edi::special_functions(),
            detail::passive_class_data::m_base_classes,
            detail::passive_class_data::m_properties,
            detail::passive_class_data::m_functions,
            nullptr)
    {
    }

} // namespace cambrian
