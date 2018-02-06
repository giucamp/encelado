
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "ediacaran/core/array_view.h"
#include "ediacaran/reflection/qualified_type_ptr.h"
#include "ediacaran/reflection/type.h"
#include <ediacaran/core/array.h>
#include <utility>

namespace edi
{
    class parameter
    {
      public:
        constexpr parameter(qualified_type_ptr const i_qualified_type)
            : m_qualified_type(i_qualified_type)
        {
        }

        constexpr qualified_type_ptr const & qualified_type() const noexcept
        {
            return m_qualified_type;
        }

      private:
        qualified_type_ptr const m_qualified_type;
    };

} // namespace edi
