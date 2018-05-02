
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "cambrian/data/serializer.h"

namespace cambrian
{
    binary_writer::Level::Level(const raw_ptr & i_object)
        : m_prop_iterator(inspect_properties(i_object).begin()), m_element_iterator(i_object)
    {
    }

    binary_writer::binary_writer(type_registry & i_type_registry, const raw_ptr & i_source_object)
        : m_type_registry(i_type_registry), m_curr_object(i_source_object)
    {
    }

    bool binary_writer::write_object(
      byte_writer & i_dest, const raw_ptr & i_source_object, write_object_flags i_flags)
    {
        auto const & qualified_type = i_source_object.qualified_type();
        if (qualified_type.indirection_levels() != 0)
        {
            except<std::runtime_error>("attempt to serialize a pointer");
        }

        auto const & actual_type = *qualified_type.final_type();
        if (actual_type.is_fundamental())
        {
            auto size = actual_type.size();
            if (i_flags == wo_add_marker)
                size += sizeof(data_marker);

            if (static_cast<ptrdiff_t>(size) > i_dest.remaining_size())
                return false;

            if (i_flags == wo_add_marker)
            {
                auto const & type_data = m_type_registry.get_type_data(actual_type);
                data_marker  marker;
                marker.m_type_id = type_data.m_id;
                marker.m_count   = 1;
                i_dest.write_unchecked(&marker, sizeof(marker));
            }

            i_dest.write_unchecked(i_source_object.object(), actual_type.size());
            return true;
        }
        else
        {
            m_stack.emplace_back(i_source_object);
            return true;
        }
    }

    binary_writer::result binary_writer::step(byte_writer & i_dest)
    {
        if (m_curr_object)
        {
            if (!write_object(i_dest, m_curr_object, wo_add_marker))
                return more_space;
            m_curr_object = {};
        }

        while (!m_stack.empty())
        {
            auto & level = m_stack.back();

            if (level.m_move_next_property)
            {
                CAMBRIAN_ASSERT(level.m_prop_iterator != end_marker);
                ++level.m_prop_iterator;
                level.m_move_next_property = false;
            }

            if (level.m_move_next_element)
            {
                CAMBRIAN_ASSERT(level.m_element_iterator != end_marker);
                ++level.m_element_iterator;
                level.m_move_next_element = false;
            }

            if (level.m_prop_iterator != end_marker)
            {
                if (!write_object(i_dest, (*level.m_prop_iterator).get_value(), wo_none))
                    return more_space;
                level.m_move_next_property = true;
            }
            else if (level.m_element_iterator != end_marker)
            {
                if (!write_object(i_dest, *level.m_element_iterator, wo_add_marker))
                    return more_space;
                level.m_move_next_element = true;
            }
            else
            {
                m_stack.pop_back();
            }
        }
        return finished;
    }

} // namespace cambrian
