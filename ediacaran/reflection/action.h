#pragma once

#include "ediacaran/core/array_view.h"
#include "ediacaran/core/comma_separated_names.h"
#include "ediacaran/reflection/parameter.h"
#include "ediacaran/reflection/qualified_type_ptr.h"
#include "ediacaran/reflection/type.h"
#include <ediacaran/core/array.h>
#include <utility>

namespace ediacaran
{
    class action : public symbol
    {
      public:
        using invoke_function =
          void (*)(void * i_dest_object, void * o_return_value_dest, const void * const * i_parameters);

        constexpr action(
          const char * i_name, qualified_type_ptr const & i_return_qualified_type,
          array_view<const parameter> const & i_parameters, const char * i_parameter_names,
          invoke_function i_invoke_function)
            : symbol(i_name), m_invoke_function(i_invoke_function), m_return_qualified_type(i_return_qualified_type),
              m_parameters(i_parameters), m_parameter_names(i_parameter_names)
        {
            size_t parameter_names_count = 0;
            for (auto it = m_parameter_names.cbegin(); it != m_parameter_names.end(); it++)
                parameter_names_count++;
            if (m_parameters.size() != parameter_names_count)
            {
                except<std::runtime_error>(
                  "The action ",
                  i_name,
                  " has ",
                  m_parameters.size(),
                  " parameters, ",
                  parameter_names_count,
                  " parameter names provided");
            }
        }

        constexpr qualified_type_ptr const & qualified_return_type() const noexcept { return m_return_qualified_type; }

        constexpr array_view<const parameter> const & parameters() const noexcept { return m_parameters; }

        void invoke(void * i_dest_object, void * o_return_value_dest, const void * const * i_parameters) const
        {
            (*m_invoke_function)(i_dest_object, o_return_value_dest, i_parameters);
        }

        constexpr comma_separated_names const & parameter_names() const noexcept { return m_parameter_names; }

      private:
        invoke_function const             m_invoke_function;
        qualified_type_ptr const          m_return_qualified_type;
        array_view<const parameter> const m_parameters;
        comma_separated_names const       m_parameter_names;
    };

    namespace detail
    {
        template <typename METHOD_TYPE> struct MethodTraits;

        template <typename OWNING_CLASS, typename RETURN_TYPE, typename... PARAMETER_TYPE>
        struct MethodTraits<RETURN_TYPE (OWNING_CLASS::*)(PARAMETER_TYPE...)>
        {
            constexpr static size_t parameter_count = sizeof...(PARAMETER_TYPE);
        };

        template <typename METHOD_TYPE, METHOD_TYPE METHOD, typename INDEX_SEQUENCE> struct ActionInvoker;

        template <
          typename OWNING_CLASS, typename RETURN_TYPE, typename... PARAMETER_TYPE,
          RETURN_TYPE (OWNING_CLASS::*METHOD)(PARAMETER_TYPE...), size_t... INDEX>
        struct ActionInvoker<RETURN_TYPE (OWNING_CLASS::*)(PARAMETER_TYPE...), METHOD, std::index_sequence<INDEX...>>
        {
            using return_type = RETURN_TYPE;

            constexpr static array<parameter, sizeof...(PARAMETER_TYPE)> parameters = {
              parameter{get_qualified_type<PARAMETER_TYPE>()}...};

            static void func(void * i_dest_object, void * o_return_value_dest, const void * const * i_parameters)
            {
                EDIACARAN_ASSERT(o_return_value_dest != nullptr);
                auto & object = *static_cast<OWNING_CLASS *>(i_dest_object);
                new (o_return_value_dest)
                  RETURN_TYPE((object.*METHOD)(*static_cast<const PARAMETER_TYPE *>(i_parameters[INDEX])...));
            }
        };

        template <
          typename OWNING_CLASS, typename... PARAMETER_TYPE, void (OWNING_CLASS::*METHOD)(PARAMETER_TYPE...),
          size_t... INDEX>
        struct ActionInvoker<void (OWNING_CLASS::*)(PARAMETER_TYPE...), METHOD, std::index_sequence<INDEX...>>
        {
            using return_type = void;

            constexpr static array<parameter, sizeof...(PARAMETER_TYPE)> parameters = {
              parameter{get_qualified_type<PARAMETER_TYPE>()}...};

            static void func(void * i_dest_object, void * o_return_value_dest, const void * const * i_parameters)
            {
                auto & object = *static_cast<OWNING_CLASS *>(i_dest_object);
                (object.*METHOD)(*static_cast<const PARAMETER_TYPE *>(i_parameters[INDEX])...);
            }
        };

        template <typename METHOD_TYPE, METHOD_TYPE METHOD>
        constexpr action make_action(const char * i_name, const char * i_parameter_names)
        {
            using action_invoker = detail::ActionInvoker<
              METHOD_TYPE,
              METHOD,
              std::make_index_sequence<detail::MethodTraits<METHOD_TYPE>::parameter_count>>;
            return action(
              i_name,
              get_qualified_type<typename action_invoker::return_type>(),
              action_invoker::parameters,
              i_parameter_names,
              &action_invoker::func);
        }
    }

} // namespace ediacaran
