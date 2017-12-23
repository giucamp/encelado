
#include <ediacaran/utils/inspect.h>

namespace ediacaran
{
    property_inspector::iterator::iterator(const raw_ptr & i_target) noexcept : m_target(i_target.full_indirection())
    {
        auto const final_type = m_target.type().final_type();
        if (final_type->is_class())
        {
            m_class = static_cast<const class_type *>(final_type);
            m_subobject = m_target.object();
            m_property = m_class->properties().data();
            if (m_class->properties().empty())
            {
                next_base();
            }
        }
    }

    void property_inspector::iterator::next_base() noexcept
    {
        if (m_class == m_target.type().final_type())
        {
            EDIACARAN_INTERNAL_ASSERT(m_base_index == 0);
        }
        else
        {
            m_base_index++;
        }

        for (;;)
        {
            auto const complete_class = static_cast<const class_type *>(m_target.type().final_type());
            if (m_base_index < complete_class->base_classes().size())
            {
                auto const & base = complete_class->base_classes()[m_base_index];
                m_class = &base.get_class();
                m_subobject = base.up_cast(m_target.object());
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
        auto value = const_cast<void*>(m_property->get_inplace(m_subobject));
        if(value == nullptr)
        {
            value = m_dyn_value.uninitialized_allocate(property_type);
            char error_msg[512];
            char_writer error_writer(error_msg);
            if(m_property->get(m_subobject, value, error_writer))

        }
        return raw_ptr(value, property_type);
    }

} // namespace ediacaran