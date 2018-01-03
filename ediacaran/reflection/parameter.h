#pragma once

#include "ediacaran/core/array_view.h"
#include "ediacaran/reflection/qualified_type_ptr.h"
#include "ediacaran/reflection/type.h"
#include <array>
#include <utility>

namespace ediacaran
{
    class parameter : public symbol_t
    {
    public:
        constexpr parameter(const char * i_name, qualified_type_ptr const i_qualified_type)
            : symbol_t(i_name), m_qualified_type(i_qualified_type)
        {
        }

        constexpr qualified_type_ptr const & qualified_type() const noexcept { return m_qualified_type; }

    private:
        qualified_type_ptr const m_qualified_type;
    };

} // namespace ediacaran
