#pragma once

#include "ediacaran/reflection/reflection.h"
#include <vector>
#include <string>

/*namespace std
{
    template <typename TYPE> class allocator;
    template <typename TYPE, typename ALLOCATOR> class vector;
    template < typename CHAR, typename TRAITS, typename ALLOCATOR> class basic_string;
}*/

namespace ediacaran
{
    template <typename... PARAMS> struct non_intrusive_reflection< std::allocator<PARAMS...> >
    {
        static constexpr auto reflect()
        {
            return make_class< std::allocator<PARAMS...> >("std::allocator");
        }
    };

    template <typename... PARAMS> struct non_intrusive_reflection< std::vector<PARAMS...> >
    {
        static constexpr auto reflect()
        {
            return make_class< std::vector<PARAMS...> >("std::vector");
        }
    };

    template <typename... PARAMS> struct non_intrusive_reflection< std::char_traits<PARAMS...> >
    {
        static constexpr auto reflect()
        {
            return make_class< std::char_traits<PARAMS...> >("std::char_traits");
        }
    };

    template <typename... PARAMS> struct non_intrusive_reflection< std::basic_string<PARAMS...> >
    {
        static constexpr auto reflect()
        {
            return make_class< std::basic_string<PARAMS...> >("std::char_traits");
        }
    };

    /*constexpr auto reflect(std::string ** i_ptr)
    {
        constexpr auto const template_arguments = make_template_arguments();
        return make_class<
          std::remove_reference_t<decltype(**i_ptr)>,
          char_array_size(template_arguments)>("std::string", template_arguments);
    }*/

} // namespace ediacaran
