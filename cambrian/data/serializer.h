
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "cambrian/cambrian_common.h"
#include "ediacaran/core/array_view.h"
#include "ediacaran/core/byte_writer.h"
#include "ediacaran/core/expected.h"
#include "ediacaran/utils/inspect.h"
#include "ediacaran/utils/raw_ptr.h"
#include "ediacaran/utils/universal_iterator.h"
#include <deque>

namespace cambrian
{

    class binary_writer
    {
      public:
        binary_writer(const raw_ptr & i_source_object);

        enum result
        {
            finished,
            more_space
        };

        EDI_NODISCARD result step(byte_writer & i_dest);

      private:
        bool write_object(byte_writer & i_dest, const raw_ptr & i_source_object);

      private:
        struct Level
        {
            property_inspector::iterator m_prop_iterator;
            universal_iterator           m_element_iterator;
            Level(const raw_ptr & i_object);
        };
        std::deque<Level> m_stack;
        raw_ptr           m_curr_object;
    };

} // namespace cambrian
