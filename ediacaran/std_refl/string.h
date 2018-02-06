
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "ediacaran/reflection/reflection.h"
#include "ediacaran/std_refl/allocator.h"
#include <string>

namespace edi
{
    template <typename... PARAMS> struct non_intrusive_reflection<std::char_traits<PARAMS...>>
    {
        static constexpr auto reflect()
        {
            return make_class<std::char_traits<PARAMS...>>("std::char_traits");
        }
    };

    template <typename... PARAMS>
    struct is_contogous_container<std::basic_string<PARAMS...>> : std::true_type
    {
    };

    template <typename... PARAMS> struct non_intrusive_reflection<std::basic_string<PARAMS...>>
    {
        static constexpr auto reflect()
        {
            return make_class<std::basic_string<PARAMS...>>("std::char_traits");
        }
    };

    /*constexpr auto reflect(std::string ** i_ptr)
    {
        constexpr auto const template_arguments = make_template_arguments();
        return make_class<
          std::remove_reference_t<decltype(**i_ptr)>,
          char_array_size(template_arguments)>("std::string", template_arguments);
    }*/

} // namespace edi
