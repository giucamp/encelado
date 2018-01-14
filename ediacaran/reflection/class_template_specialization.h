#pragma once
#include "ediacaran/core/comma_separated_names.h"
#include "ediacaran/reflection/class_type.h"
#include <tuple>

namespace ediacaran
{
    class class_template_specialization : public class_type
    {
      public:
        constexpr class_template_specialization(
          const char * const                     i_name,
          const char *                           i_template_parameter_names,
          size_t                                 i_size,
          size_t                                 i_alignment,
          const special_functions &              i_special_functions,
          const array_view<const base_class> &   i_base_classes,
          const array_view<const property> &     i_properties,
          const array_view<const action> &       i_actions,
          const array_view<const parameter> &    i_template_parameters,
          const array_view<const void * const> & i_template_arguments)
            : class_type(
                i_name,
                i_size,
                i_alignment,
                i_special_functions,
                i_base_classes,
                i_properties,
                i_actions),
              m_template_parameter_names(i_template_parameter_names),
              m_template_parameters(i_template_parameters),
              m_template_arguments(i_template_arguments)
        {
        }

        constexpr comma_separated_names const & get_template_parameter_names() const noexcept
        {
            return m_template_parameter_names;
        }

        constexpr array_view<const parameter> const & template_parameters() const noexcept { return m_template_parameters; }
        
        constexpr array_view<const void * const> const & template_arguments() const noexcept { return m_template_arguments; }

      private:
        comma_separated_names const          m_template_parameter_names;
        array_view<const parameter> const    m_template_parameters;
        array_view<const void * const> const m_template_arguments;
    };

    template <typename... TYPES> class template_arguments
    {
      public:
        constexpr template_arguments(const char * i_parameter_names, TYPES &&... i_arguments)
            : m_parameter_names(i_parameter_names), m_arguments(std::move(i_arguments)...)
        {
        }

        constexpr static size_t size = sizeof...(TYPES);

        constexpr const char * parameter_names() const noexcept { return m_parameter_names; }

        template <size_t INDEX> constexpr const auto & get() const
        {
            return std::get<INDEX>(m_arguments);
        }

        constexpr std::tuple<TYPES...> const & as_tuple() const noexcept { return m_arguments; }

        friend constexpr char_writer & operator << (char_writer & o_dest, const template_arguments<TYPES...> & i_source)
        {
            return o_dest << '<' << i_source.as_tuple() << '>';
        }

        constexpr size_t name_len() const noexcept
        {
            char_writer writer;
            writer << *this;
            EDIACARAN_ASSERT(writer.remaining_size() <= 0);
            return static_cast<size_t>(-writer.remaining_size());
        }

      private:
        const char * const         m_parameter_names;
        std::tuple<TYPES...> const m_arguments;
    };

    template <typename... TYPES>
    constexpr auto make_template_arguments(const char * i_parameter_names, TYPES &&... i_arguments)
    {
        return template_arguments<TYPES...>(i_parameter_names, std::move(i_arguments)...);
    }

    constexpr auto make_template_arguments() { return template_arguments<>(""); }

} // namespace ediacaran
