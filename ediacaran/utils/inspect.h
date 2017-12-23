#pragma once
#include <ediacaran/reflection/class_type.h>
#include <ediacaran/utils/dyn_value.h>

namespace ediacaran
{
    class property_inspector
    {
      public:
        property_inspector(const raw_ptr & i_target) noexcept : m_target(i_target.full_indirection()) {}

        class iterator;

        class content
        {
          public:
            content(const class iterator & m_parent) noexcept
                : m_parent(m_parent)
            {
            }

            class_type const & owning_class() const noexcept { return *m_parent.m_class; }

            property const & property() const noexcept { return *m_parent.m_property; }

            raw_ptr get_value() const { return m_parent.get_prop_value(); }

          private:
            const class iterator & m_parent;
        };

        class end_marker
        {
        };

        class iterator
        {
          public:
            iterator(const raw_ptr & i_target) noexcept;

            content operator*() const noexcept { return content(*this); }

            iterator & operator++() noexcept
            {
                m_property++;
                if (m_property == m_class->properties().end())
                {
                    next_base();
                }
                return *this;
            }

            iterator operator++(int)
            {
                auto result(*this);
                operator++();
                return result;
            }

            bool operator==(const end_marker &) const noexcept { return m_property == nullptr; }

            bool operator!=(const end_marker &) const noexcept { return m_property != nullptr; }

            bool operator==(const iterator & i_source) const noexcept { return m_property == i_source.m_property; }

            bool operator!=(const iterator & i_source) const noexcept { return m_property != i_source.m_property; }

          private:
            friend class content;
            void next_base() noexcept;
            raw_ptr get_prop_value();

          private:
            void * m_subobject = nullptr;
            const class_type * m_class = nullptr;
            const property * m_property = nullptr;
            size_t m_base_index = 0;
            raw_ptr const m_target;
            dyn_value m_dyn_value;
        };

        iterator begin() const noexcept { return iterator(m_target); }

        end_marker end() const noexcept { return end_marker{}; }

      private:
        raw_ptr const m_target;
    };

    inline property_inspector inspect_properties(const raw_ptr & i_target) { return property_inspector(i_target); }

} // namespace ediacaran