
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
                    throw std::runtime_error(error_msg);
                }
            });
        }
        return raw_ptr(value, property_type);
    }

    const char * property_inspector::iterator::get_string_value()
    {
        if (!m_property->can_get())
        {
            return "(can't get)";
        }

        auto const min_size = sizeof(void *) * 4;
        if (m_char_buffer.size() < min_size)
        {
            m_char_buffer.resize(min_size);
        }

        auto const value = get_prop_value().full_indirection();
        if (value.object() == nullptr)
        {
            return "(null)";
        }
        else
        {
            auto const final_type = value.qualified_type().final_type();

            char_writer writer(m_char_buffer.data(), m_char_buffer.size());

            final_type->stringize(value.object(), writer);
            if (writer.remaining_size() < 0)
            {
                m_char_buffer.resize(m_char_buffer.size() - writer.remaining_size());
                writer = char_writer(m_char_buffer.data(), m_char_buffer.size());
                final_type->stringize(value.object(), writer);
            }

            return m_char_buffer.data();
        }
    }

    raw_ptr action_inspector::iterator::invoke(const array_view<const raw_ptr> & i_arguments)
    {
        auto const & parameters = m_action->parameters();

        if(i_arguments.size() != parameters.size())
            throw mismatching_arguments(to_string("The action ", m_action->name(), " expects ",
                parameters.size(), ", ", i_arguments.size(), " were provided"));
        
        std::vector<void*> arguments(i_arguments.size());
        for(size_t i = 0; i < i_arguments.size(); i++)
        {
            if (i_arguments[i].qualified_type() != parameters[i].qualified_type())
            {
                throw mismatching_arguments(to_string("The argument ", i, " (", parameters[i].name(),
                    ") of the action ", m_action->name(), " is expected to have type ",
                    parameters[i].qualified_type(), ", a ", i_arguments[i].qualified_type(), " was provided"));
            }
            arguments[i] = const_cast<void*>(i_arguments[i].object());
        }

        auto const value = m_dyn_value.manual_construct(m_action->qualified_return_type(), [&](void * i_dest) {
            m_action->invoke(m_subobject, i_dest, arguments.data());
        });
        return raw_ptr(value, m_action->qualified_return_type());
    }

} // namespace ediacaran