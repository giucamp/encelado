#pragma once

#include "ediacaran/reflection/reflection.h"
#include <memory>

namespace ediacaran
{
    template <typename... PARAMS> struct non_intrusive_reflection<std::allocator<PARAMS...>>
    {
        static constexpr auto reflect()
        {
            return make_class<std::allocator<PARAMS...>>("std::allocator");
        }
    };

} // namespace ediacaran
