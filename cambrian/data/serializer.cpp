
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

    binary_writer::binary_writer(const raw_ptr & i_source_object) : m_curr_object(i_source_object)
    {
    }

    bool binary_writer::write_object(byte_writer & i_dest, const raw_ptr & i_source_object)
    {
        auto const & qualified_type = i_source_object.qualified_type();
        if (qualified_type.indirection_levels() != 0)
        {
            except<std::runtime_error>("attempt to serialize a pointer");
        }

        if (qualified_type.final_type()->is_fundamental())
        {
            return i_dest.write_all_or_none(
              i_source_object.object(), qualified_type.final_type()->size());
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
            if (!write_object(i_dest, m_curr_object))
                return more_space;
            m_curr_object = {};
        }

        while (!m_stack.empty())
        {
            auto & level = m_stack.back();

            if (level.m_prop_iterator != end_marker)
            {
                if (!write_object(i_dest, (*level.m_prop_iterator).get_value()))
                    return more_space;
                ++level.m_prop_iterator;
            }
            else if (level.m_element_iterator != end_marker)
            {
                if (!write_object(i_dest, *level.m_element_iterator))
                    return more_space;
                ++level.m_element_iterator;
            }
            else
            {
                m_stack.pop_back();
            }
        }
        return finished;
    }

} // namespace cambrian
