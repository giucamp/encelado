
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "cambrian/cambrian_common.h"
#include "cambrian/data/type_registry.h"
#include "ediacaran/core/array_view.h"
#include "ediacaran/core/byte_writer.h"
#include "ediacaran/core/expected.h"
#include "ediacaran/utils/inspect.h"
#include "ediacaran/utils/raw_ptr.h"
#include "ediacaran/utils/universal_iterator.h"
#include <deque>
#include <limits>

namespace cambrian
{
    struct data_marker
    {
        type_id  m_type_id = 0;
        uint16_t m_count   = 0;
        uint16_t m_flags   = 0;

        constexpr static uint16_t s_max_count = std::numeric_limits<uint16_t>::max();

        enum flags
        {
            flag_none            = 0,
            flag_begin_comtainer = 1 << 0,
            flag_end_comtainer   = 1 << 1,
        };

        data_marker() : m_type_id(0), m_count(0), m_flags(0) {}
    };

    class binary_writer
    {
      public:
        binary_writer(type_registry & i_type_registry, const raw_ptr & i_source_object);

        enum result
        {
            finished,
            more_space
        };

        EDI_NODISCARD result step(byte_writer & i_dest);

      private:
        bool push_level(const raw_ptr & i_source_object);

        bool write_prop_value(byte_writer & i_dest, const raw_ptr & i_source_object);

      private:
        struct Level
        {
            property_inspector::iterator m_prop_iterator;
            universal_iterator           m_element_iterator;
            bool                         m_move_next_property = false;
            bool                         m_move_next_element  = false;
            bool                         m_first_element      = true;
            data_marker *                marker               = nullptr;
            Level(const raw_ptr & i_object);
        };
        type_registry &   m_type_registry;
        std::deque<Level> m_stack;
        raw_ptr           m_curr_object;
    };

} // namespace cambrian
