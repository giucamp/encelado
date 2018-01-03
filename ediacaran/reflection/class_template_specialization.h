#pragma once

#include "ediacaran/reflection/class_type.h"

namespace ediacaran
{
    class class_template_specialization : public class_type
    {
    public:
        constexpr class_template_specialization(const char * const i_name, size_t i_size, size_t i_alignment,
            const special_functions & i_special_functions, const array_view<const base_class> & i_base_classes,
            const array_view<const property> & i_properties, const array_view<const action> & i_actions, 
            const array_view<const parameter> & i_template_parameters,
            const array_view<const void * const> & i_template_arguments)
                : class_type(i_name, i_size, i_alignment, i_special_functions, i_base_classes, i_properties, i_actions),
                  m_template_parameters(i_template_parameters), m_template_arguments(i_template_arguments)
        {
        }

    private:
        array_view<const parameter> const m_template_parameters;
        array_view<const void * const> const m_template_arguments;
    };

} // namespace ediacaran
