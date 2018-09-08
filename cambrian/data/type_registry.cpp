
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "cambrian/data/type_registry.h"
#include <algorithm>
#include <string>
#include <vector>

namespace cambrian
{
    struct type_registry::SerializedClassData
    {
        type_id                  m_type_id = 0;
        std::string              m_name;
        size_t                   m_size = 0;
        std::vector<base_class>  m_base_classes;
        std::vector<property>    m_properties;
        std::vector<function>    m_functions;
        std::vector<std::string> m_property_names;

        SerializedClassData(
          type_registry * i_type_registry, const class_type & i_source_class, type_id i_type_id)
            : m_type_id(i_type_id)
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
                auto const & passive_base =
                  i_type_registry->get_type_data(base.get_class()).m_active_type;
                CAMBRIAN_ASSERT(passive_base.is_class());

                m_base_classes.emplace_back(static_cast<const class_type &>(passive_base), nullptr);
                add_properties(i_type_registry, base.get_class().properties());
            }
        }

      private:
        void add_properties(
          type_registry * i_type_registry, const array_view<const property> & i_properties)
        {
            for (auto const & prop : i_properties)
            {
                auto const raw_type = prop.qualified_type().final_type();

                auto const & prop_passive_type =
                  i_type_registry->get_type_data(*raw_type).m_active_type;

                auto const prop_name = m_property_names.emplace_back(prop.name());

                m_properties.emplace_back(
                  prop_name.c_str(), qualified_type_ptr(&prop_passive_type), m_size);

                m_size += prop_passive_type.size();
            }
        }
    };

    struct type_registry::SerializedClass
    {
      public:
        SerializedClass(
          type_registry * i_type_registry, const class_type & i_source_class, type_id i_type_id)
            : m_class_data(i_type_registry, i_source_class, i_type_id),
              m_class(
                m_class_data.m_name.c_str(),
                m_class_data.m_size,
                1,
                edi::special_functions(),
                m_class_data.m_base_classes,
                m_class_data.m_properties,
                m_class_data.m_functions,
                nullptr)
        {
        }

        SerializedClassData m_class_data;
        class_type          m_class;
    };


    type_registry::type_data type_registry::get_type_data(const type & i_source_type)
    {
        if (i_source_type.is_fundamental())
        {
            return {i_source_type, 0, i_source_type, i_source_type.size()};
        }
        else
        {
            auto const & source_class = static_cast<const class_type &>(i_source_type);
            auto &       slot         = m_classes[&source_class];
            if (!slot)
            {
                slot = std::make_unique<SerializedClass>(this, source_class, m_next_type_id);
                m_next_type_id++;
            }
            return {i_source_type,
                    slot->m_class_data.m_type_id,
                    slot->m_class,
                    slot->m_class_data.m_size};
        }
    }

    type_registry::type_registry() = default;

    type_registry::~type_registry() = default;

} // namespace cambrian
