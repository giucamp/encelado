
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

    bool binary_writer::push_level(const raw_ptr & i_source_object)
    {
        auto const & qualified_type = i_source_object.qualified_type();
        if (qualified_type.indirection_levels() != 0)
        {
            except<std::runtime_error>("attempt to serialize a pointer");
        }

        auto const & actual_type = *qualified_type.final_type();
        if (!actual_type.is_fundamental())
        {
            return false;
        }
        else
        {
            m_stack.emplace_back(i_source_object);
            return true;
        }
    }

    bool binary_writer::write_prop_value(byte_writer & i_dest, const raw_ptr & i_source_object)
    {
        if (push_level(i_source_object))
        {
        }
        else
        {
            if (!i_dest.write_all_or_none(
                  i_source_object.object(), i_source_object.qualified_type().final_type()->size()))
            {
                return binary_writer::more_space;
            }
        }
    }

    binary_writer::result binary_writer::step(byte_writer & i_dest)
    {
        for (auto prop : inspect_properties(m_curr_object))
        {
            if (!write_prop_value(i_dest, prop.get_value()))
                return binary_writer::more_space;
        }

        void *      marker_dest = nullptr;
        data_marker curr_marker;
        auto        commit_marker = [&] {
            if (marker_dest != nullptr)
            {
                memcpy(marker_dest, &curr_marker, sizeof(curr_marker));
            }
        };

        bool         first_element     = true;
        type * const last_element_type = nullptr;
        for (universal_iterator it(m_curr_object); it != end_marker; ++it)
        {
            auto const & qualified_type = (*it).qualified_type();
            CAMBRIAN_ASSERT(qualified_type.final_type() != nullptr);

            if (
              qualified_type.final_type() != last_element_type ||
              curr_marker.m_count >= data_marker::s_max_count)
            {
                commit_marker();

                auto const & type_data =
                  m_type_registry.get_type_data(*qualified_type.final_type());
                curr_marker           = {};
                curr_marker.m_type_id = type_data.m_id;

                marker_dest = i_dest.skip(sizeof(data_marker));

                if (first_element)
                {
                    curr_marker.m_flags |= data_marker::flag_begin_comtainer;
                    first_element = false;
                }
            }

            if (!write_prop_value(i_dest, *it))
                return binary_writer::more_space;

            curr_marker.m_count++;
        }

        curr_marker.m_flags |= data_marker::flag_end_comtainer;
        commit_marker();

        return binary_writer::finished;
    }

    /*binary_writer::result binary_writer::step(byte_writer & i_dest)
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
                if (level.m_first_element)
                    data_marker::flag_begin_comtainer;

                if (!write_object(i_dest, *level.m_element_iterator, wo_add_marker))
                    return more_space;
                level.m_first_element     = false;
                level.m_move_next_element = true;
            }
            else
            {
                m_stack.pop_back();
            }
        }
        return finished;
    }*/

} // namespace cambrian
