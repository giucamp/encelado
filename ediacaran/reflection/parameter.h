#pragma once

#include "ediacaran/core/array_view.h"
#include "ediacaran/reflection/qualified_type_ptr.h"
#include "ediacaran/reflection/type.h"
#include <array>
#include <utility>

namespace ediacaran
{
    class parameter
    {
      public:
        constexpr parameter(qualified_type_ptr const i_qualified_type) : m_qualified_type(i_qualified_type) {}

        constexpr qualified_type_ptr const & qualified_type() const noexcept { return m_qualified_type; }

      private:
        qualified_type_ptr const m_qualified_type;
    };

} // namespace ediacaran
