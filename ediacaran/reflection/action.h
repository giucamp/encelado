#pragma once

#include "ediacaran/core/array_view.h"
#include "ediacaran/reflection/qualified_type_ptr.h"
#include "ediacaran/reflection/type.h"
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

        constexpr qualified_type_ptr const & qualified_type() const noexcept
        {
            return m_qualified_type;
        }

    private:
        qualified_type_ptr const m_qualified_type;
    };

    namespace detail
    {
        template <typename METHOD_TYPE>
            struct MethodTraits
        {

        };

        template <typename OWNING_CLASS, typename RETURN_TYPE, typename... PARAMETER_TYPE>
            struct MethodTraits<RETURN_TYPE (OWNING_CLASS::*)(PARAMETER_TYPE...)>
        {
            constexpr static size_t parameter_count = sizeof...(PARAMETER_TYPE);
        };

        template <typename METHOD_TYPE, METHOD_TYPE METHOD, typename INDEX_SEQUENCE>
            struct ActionInvoker;

        template <typename OWNING_CLASS, typename RETURN_TYPE, typename... PARAMETER_TYPE, 
                RETURN_TYPE (OWNING_CLASS::*METHOD)(PARAMETER_TYPE...), size_t... INDEX>
            struct ActionInvoker<RETURN_TYPE (OWNING_CLASS::*)(PARAMETER_TYPE...), METHOD, std::index_sequence<INDEX...>>
        {
            static void func(void * i_dest_object, void * o_return_value_dest, const void * const * i_parameters)
            {
                EDIACARAN_ASSERT(o_return_value_dest != nullptr);
                auto & object = *static_cast<OWNING_CLASS*>(i_dest_object);
                new(o_return_value_dest) RETURN_TYPE((object.*METHOD)(
                    *static_cast<const PARAMETER_TYPE*>(i_parameters[INDEX])...
                ));
            }
        };

        template <typename OWNING_CLASS, typename... PARAMETER_TYPE, 
                void (OWNING_CLASS::*METHOD)(PARAMETER_TYPE...), size_t... INDEX>
            struct ActionInvoker<void (OWNING_CLASS::*)(PARAMETER_TYPE...), METHOD, std::index_sequence<INDEX...>>
        {
            static void func(void * i_dest_object, void * o_return_value_dest, const void * const * i_parameters)
            {
                EDIACARAN_ASSERT(o_return_value_dest == nullptr);
                auto & object = *static_cast<OWNING_CLASS*>(i_dest_object);
                (object.*METHOD)(
                    *static_cast<const PARAMETER_TYPE*>(i_parameters[INDEX])...
                );
            }
        };
    }

    class action : public symbol_t
    {
    public:

        using invoke_function = void (*)(void * i_dest_object, void * o_return_value_dest, const void * const * i_parameters);

        constexpr action(const char * i_name, invoke_function i_invoke_function)
            : symbol_t(i_name), m_invoke_function(i_invoke_function), m_parameters()
        {

        }
        
        constexpr array_view<const parameter> parameters() const noexcept
        {
            return m_parameters;
        }        

        void invoke(void * i_dest_object, void * o_return_value_dest, const void * const * i_parameters) const
        {
            (*m_invoke_function)(i_dest_object, o_return_value_dest, i_parameters);
        }

    private:
        invoke_function const m_invoke_function;
        array_view<const parameter> const m_parameters;
    };

    template <typename METHOD_TYPE, METHOD_TYPE METHOD> 
        constexpr action make_action(const char * i_name)
    {
        using action_invoker = detail::ActionInvoker<METHOD_TYPE, METHOD, std::make_index_sequence<detail::MethodTraits<METHOD_TYPE>::parameter_count>>;
        return action(i_name, &action_invoker::func);
    }


} // namespace ediacaran
