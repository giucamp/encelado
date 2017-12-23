#pragma once
#include <ediacaran/reflection/class_type.h>
#include <ediacaran/utils/dyn_value.h>

namespace ediacaran
{
    class property_inspector
    {
    public:

        property_inspector(const raw_ptr & i_target) noexcept
            : m_target(i_target.full_indirection())
        {
            
        }

        class content
        {
        public:

            content(class_type const & i_owning_class, property const & i_property, void * i_object)
                : m_property(i_property), m_object(i_object), m_owning_class(i_owning_class)
            {
            }

            class_type const & owning_class() const noexcept
            {
                return m_owning_class;
            }

            property const & property() const noexcept
            {
                return m_property;
            }

            raw_ptr value() const
            {
                return raw_ptr();
            }

        private:
            class property const & m_property;
            void * const m_object;
            class_type const & m_owning_class;
        };

        class end_marker {};

        class iterator
        {
        public:

        iterator(const raw_ptr & i_target) noexcept
            : m_target(i_target.full_indirection())
        {
            auto const final_type = m_target.type().final_type();
            if(final_type->is_class())
            {
                m_class = static_cast<const class_type*>(final_type);
                m_subobject = m_target.object(); 
                m_property = m_class->properties().data();
                if(m_class->properties().empty())
                    next_base();
            }            
        }

        content operator * () const noexcept
        {
            return content(*m_class, *m_property, m_subobject);
        }

        iterator & operator ++ ()
        {
            m_property++;
            if(m_property == m_class->properties().end())
            {
                next_base();
            }
            return *this;
        }

        iterator operator ++ (int)
        {
            auto result(*this);
            operator ++ ();
            return result;
        }

        bool operator == (const end_marker &) const noexcept
        {
            return m_property == nullptr;
        }

        bool operator != (const end_marker &) const noexcept
        {
            return m_property != nullptr;
        }

        bool operator == (const iterator & i_source) const noexcept
        {
            return m_property == i_source.m_property;
        }

        bool operator != (const iterator & i_source) const noexcept
        {
            return m_property != i_source.m_property;
        }

    private:

        void next_base()
        {
            for(;;)
            {
                if(m_class == m_target.type().final_type())
                {
                    EDIACARAN_INTERNAL_ASSERT(m_base_index == 0);
                }
                else
                {
                    m_base_index++;   
                }

                auto const complete_class = static_cast<const class_type*>(m_target.type().final_type());
                if(m_base_index < complete_class->base_classes().size())
                {
                    auto const & base = complete_class->base_classes()[m_base_index];
                    m_class = &base.get_class();
                    m_subobject = base.up_cast(m_target.object());
                    if(!m_class->properties().empty())
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
            }
        }

        private:
            void * m_subobject = nullptr;
            const class_type * m_class = nullptr;
            const property * m_property = nullptr;
            size_t m_base_index = 0;
            raw_ptr const m_target;
            dyn_value m_dyn_value;
        };

        iterator begin() const noexcept
        {
            return iterator(m_target);
        }

        end_marker end() const noexcept
        {
            return end_marker{};
        }

    private:
        raw_ptr const m_target;
    };

    inline property_inspector inspect_properties(const raw_ptr & i_target)
    {
        return property_inspector(i_target);
    }

} // namespace ediacaran