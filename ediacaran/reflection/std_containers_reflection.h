#pragma once

#include "ediacaran/reflection/reflection.h"

namespace std
{
    template<typename TYPE> class allocator;
    template<typename TYPE, typename ALLOCATOR = std::allocator<TYPE>> class vector;

    template<typename CHAR, typename TRAITS = std::char_traits<CHAR>, typename ALLOCATOR = std::allocator<CHAR>> class basic_string;
}

namespace ediacaran
{    
    template <typename... PARAMS> constexpr auto reflect(std::allocator<PARAMS...> ** i_ptr)
    {
        return make_class<std::remove_reference_t<decltype(**i_ptr)>>("std::allocator");
    }

    template <typename... PARAMS> constexpr auto reflect(std::vector<PARAMS...> ** i_ptr)
    {
        return make_class<std::remove_reference_t<decltype(**i_ptr)>>("std::vector");
    }

    constexpr auto reflect(std::string ** i_ptr)
    {
        constexpr auto const template_arguments = make_template_arguments();
        return make_class<std::remove_reference_t<decltype(**i_ptr)>, char_array_size(template_arguments)>("std::string", template_arguments);
    }

} // namespace ediacaran
