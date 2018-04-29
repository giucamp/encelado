
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "cambrian/cambrian_common.h"
#include "ediacaran/reflection/class_type.h"
#include <string>
#include <vector>

namespace cambrian
{
    class passive_class;


    class type_registry
    {
      public:
        const type * get_passive_type(const type & i_source_class);
    };

    namespace detail
    {
        struct passive_class_data
        {
            std::string              m_name;
            size_t                   m_size = 0;
            std::vector<base_class>  m_base_classes;
            std::vector<property>    m_properties;
            std::vector<function>    m_functions;
            std::vector<std::string> m_property_names;

            passive_class_data(type_registry * i_type_registry, const class_type & i_source_class);

          private:
            void add_properties(
              type_registry * i_type_registry, const array_view<const property> & i_properties);
        };

    } // namespace detail

    class passive_class : private detail::passive_class_data, public class_type
    {
      public:
        explicit passive_class(type_registry * i_type_registry, const class_type & i_source_class);

      private:
      private:
        std::vector<property> m_properties;
    };

} // namespace cambrian
