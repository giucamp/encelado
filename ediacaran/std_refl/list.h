
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "ediacaran/reflection/reflection.h"
#include "ediacaran/std_refl/allocator.h"
#include <list>

namespace edi
{
    template <typename... PARAMS> struct non_intrusive_reflection<std::list<PARAMS...>>
    {
        static constexpr auto reflect() { return make_class<std::list<PARAMS...>>("std::list"); }
    };

} // namespace edi
