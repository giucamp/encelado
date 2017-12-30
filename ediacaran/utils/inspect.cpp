
#include <ediacaran/utils/inspect.h>
#include <ediacaran/core/string_builder.h>
#include <vector>

namespace ediacaran
{
    property_inspector::iterator::iterator(const raw_ptr & i_target) noexcept : m_target(i_target.full_indirection())
    {
        auto const final_type = m_target.qualified_type().final_type();
        if (final_type->is_class())
        {
            m_class = static_cast<const class_type *>(final_type);
            m_subobject = const_cast<void *>(m_target.object());
            m_property = m_class->properties().data();
            if (m_class->properties().empty())
            {
                next_base();
            }
        }
    }

    void property_inspector::iterator::next_base() noexcept
    {
        if (m_class == m_target.qualified_type().final_type())
        {
            EDIACARAN_INTERNAL_ASSERT(m_base_index == 0);
        }
        else
        {
            m_base_index++;
        }

        for (;;)
        {
            auto const complete_class = static_cast<const class_type *>(m_target.qualified_type().final_type());
            if (m_base_index < complete_class->base_classes().size())
            {
                auto const & base = complete_class->base_classes()[m_base_index];
                m_class = &base.get_class();
                m_subobject = const_cast<void *>(base.up_cast(m_target.object()));
                if (!m_class->properties().empty())
                {
                    m_property = m_class->properties().data();
                    break;
                }
            }
            else
            {
                m_property = nullptr;
                break;
            }

            m_base_index++;
        }
    }

    raw_ptr property_inspector::iterator::get_prop_value()
    {
        auto const & property_type = m_property->qualified_type();
        auto value = const_cast<void *>(m_property->get_inplace(m_subobject));
        if (value == nullptr)
        {
            value = m_dyn_value.manual_construct(property_type, [&](void * i_dest) {
                char error_msg[512];
                char_writer error_writer(error_msg);
                if (!m_property->get(m_subobject, i_dest, error_writer))
                {
                    except<std::runtime_error>(error_msg);
                }
            });
        }
        return raw_ptr(value, property_type);
    }

    dyn_value get_property_value(const raw_ptr & i_target, char_reader & i_property_name_source)
    {
        auto const property_name = try_parse_identifier(i_property_name_source);
        if (property_name.empty())
        {
            except<parse_error>("Missing property name");
        }
        for (auto const & property : inspect_properties(i_target))
        {
            if (property.name() == property_name)
            {
                return property.get_value();
            }
        }

        except<parse_error>("Property not found: ", property_name);
    }

    action_inspector::iterator::iterator(const raw_ptr & i_target) noexcept : m_target(i_target.full_indirection())
    {
        auto const final_type = m_target.qualified_type().final_type();
        if (final_type->is_class())
        {
            m_class = static_cast<const class_type *>(final_type);
            m_subobject = const_cast<void *>(m_target.object());
            m_action = m_class->actions().data();
            if (m_class->actions().empty())
            {
                next_base();
            }
        }
    }

    void action_inspector::iterator::next_base() noexcept
    {
        if (m_class == m_target.qualified_type().final_type())
        {
            EDIACARAN_INTERNAL_ASSERT(m_base_index == 0);
        }
        else
        {
            m_base_index++;
        }

        for (;;)
        {
            auto const complete_class = static_cast<const class_type *>(m_target.qualified_type().final_type());
            if (m_base_index < complete_class->base_classes().size())
            {
                auto const & base = complete_class->base_classes()[m_base_index];
                m_class = &base.get_class();
                m_subobject = const_cast<void *>(base.up_cast(m_target.object()));
                if (!m_class->actions().empty())
                {
                    m_action = m_class->actions().data();
                    break;
                }
            }
            else
            {
                m_action = nullptr;
                break;
            }

            m_base_index++;
        }
    }
    
    raw_ptr action_inspector::iterator::invoke(const array_view<const raw_ptr> & i_arguments)
    {
        auto const & parameters = m_action->parameters();

        if(i_arguments.size() != parameters.size())
        {
            except<mismatching_arguments>("The action ", m_action->name(), " expects ",
                parameters.size(), " arguments, ", i_arguments.size(), " were provided");
        }
        
        std::vector<void*> arguments(i_arguments.size());
        for(size_t i = 0; i < i_arguments.size(); i++)
        {
            if (i_arguments[i].qualified_type() != parameters[i].qualified_type())
            {
                except<mismatching_arguments>("The argument ", i, " (", parameters[i].name(),
                    ") of the action ", m_action->name(), " is expected to have type ",
                    parameters[i].qualified_type(), ", a ", i_arguments[i].qualified_type(), " was provided");
            }
            arguments[i] = const_cast<void*>(i_arguments[i].object());
        }

        auto const value = m_dyn_value.manual_construct(m_action->qualified_return_type(), [&](void * i_dest) {
            m_action->invoke(m_subobject, i_dest, arguments.data());
        });
        return raw_ptr(value, m_action->qualified_return_type());
    }

    raw_ptr action_inspector::iterator::invoke(const array_view<string_view> & i_arguments)
    {
        auto const & parameters = m_action->parameters();

        if (i_arguments.size() != parameters.size())
        {
            except<mismatching_arguments>("The action ", m_action->name(), " expects ",
                parameters.size(), " arguments, ", i_arguments.size(), " were provided");
        }

        std::vector<dyn_value> dyn_arguments;
        std::vector<void*> arguments;
        dyn_arguments.reserve(parameters.size());
        arguments.reserve(parameters.size());
        for (size_t i = 0; i < i_arguments.size(); i++)
        {
            dyn_arguments.push_back(parse_value(parameters[i].qualified_type(), i_arguments[i]));
            arguments.push_back(const_cast<void*>(dyn_arguments.back().object()));
        }
        
        auto const value = m_dyn_value.manual_construct(m_action->qualified_return_type(), [&](void * i_dest) {
            m_action->invoke(m_subobject, i_dest, arguments.data());
        });
        return raw_ptr(value, m_action->qualified_return_type());
    }

    raw_ptr action_inspector::iterator::invoke(char_reader & i_arguments_source)
    {
        auto const & parameters = m_action->parameters();

        std::vector<dyn_value> dyn_arguments;
        std::vector<void*> arguments;
        dyn_arguments.reserve(parameters.size());
        arguments.reserve(parameters.size());

        for (size_t i = 0; i < parameters.size(); i++)
        {
            try_accept(spaces, i_arguments_source);

            auto const & parameter = parameters[i];
            auto & dyn_value = dyn_arguments.emplace_back(parse_value(parameter.qualified_type(), i_arguments_source));
            arguments.push_back(const_cast<void*>(dyn_value.object()));
            
            if(i + 1 < parameters.size())
                try_accept(',', i_arguments_source);
        }
        try_accept(spaces, i_arguments_source);

        auto const value = m_dyn_value.manual_construct(m_action->qualified_return_type(), [&](void * i_dest) {
            m_action->invoke(m_subobject, i_dest, arguments.data());
        });
        return raw_ptr(value, m_action->qualified_return_type());
    }

    dyn_value invoke_action(const raw_ptr & i_target, char_reader & i_action_and_arguments)
    {
        auto const action_name = try_parse_identifier(i_action_and_arguments);
        if (action_name.empty())
        {
            except<parse_error>("Missing action name");
        }

        try_accept(spaces, i_action_and_arguments);
        if (!try_accept('(', i_action_and_arguments))
        {
            except<parse_error>("Missing '('");
        }
        for (auto const & action : inspect_actions(i_target))
        {
            if(action.name() == action_name)
            {
                return action.invoke(i_action_and_arguments);
            }
        }
        if (!try_accept(')', i_action_and_arguments))
        {
            except<parse_error>("Missing ')'");
        }
        except<parse_error>("Action not found: ", action_name);
    }

} // namespace ediacaran