#pragma once

#include "ediacaran/reflection/reflection.h"
#include "ediacaran/std_refl/allocator.h"
#include <vector>

namespace ediacaran
{
    template <typename... PARAMS>
    struct is_contogous_container<std::vector<PARAMS...>> : std::true_type
    {
    };

    template <typename... PARAMS> struct non_intrusive_reflection<std::vector<PARAMS...>>
    {
        static constexpr auto reflect()
        {
            return make_class<std::vector<PARAMS...>>("std::vector");
        }
    };

} // namespace ediacaran
