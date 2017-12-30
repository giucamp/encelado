#pragma once
#include <ediacaran/reflection/class_type.h>
#include <ediacaran/utils/dyn_value.h>
#include <vector>

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
            content(class iterator & m_parent) noexcept : m_parent(m_parent) {}

            string_view name() const noexcept { return m_parent.m_property->name(); }

            const qualified_type_ptr & qualified_type() const noexcept { return m_parent.m_property->qualified_type(); }

            class_type const & owning_class() const noexcept { return *m_parent.m_class; }

            class property const & property() const noexcept { return *m_parent.m_property; }

            raw_ptr get_value() const { return m_parent.get_prop_value(); }

            const char * get_string_value() const { return m_parent.get_string_value(); }

          private:
            class iterator & m_parent;
        };

        class end_marker
        {
        };

        class iterator
        {
          public:
            iterator(const raw_ptr & i_target) noexcept;

            content operator*() noexcept { return content(*this); }

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
            const char * get_string_value();

          private:
            void * m_subobject = nullptr;
            const class_type * m_class = nullptr;
            const property * m_property = nullptr;
            size_t m_base_index = 0;
            raw_ptr const m_target;
            dyn_value m_dyn_value;
            std::vector<char> m_char_buffer;
        };

        iterator begin() const noexcept { return iterator(m_target); }

        end_marker end() const noexcept { return end_marker{}; }

      private:
        raw_ptr const m_target;
    };

    class action_inspector
    {
    public:
        action_inspector(const raw_ptr & i_target) noexcept : m_target(i_target.full_indirection()) {}

        class iterator;

        class content
        {
        public:
            content(class iterator & m_parent) noexcept : m_parent(m_parent) {}

            string_view name() const noexcept { return m_parent.m_action->name(); }

            const qualified_type_ptr & qualified_return_type() const noexcept { return m_parent.m_action->qualified_return_type(); }

            array_view<const parameter> const & parameters() const noexcept { return m_parent.m_action->parameters(); }

            class_type const & owning_class() const noexcept { return *m_parent.m_class; }

            class action const & action() const noexcept { return *m_parent.m_action; }

            raw_ptr invoke(const array_view<const raw_ptr> & i_arguments)
            {
                return m_parent.invoke(i_arguments);
            }

        private:
            class iterator & m_parent;
        };

        class end_marker
        {
        };

        class iterator
        {
        public:
            iterator(const raw_ptr & i_target) noexcept;

            content operator*() noexcept { return content(*this); }

            iterator & operator++() noexcept
            {
                m_action++;
                if (m_action == m_class->actions().end())
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

            bool operator==(const end_marker &) const noexcept { return m_action == nullptr; }

            bool operator!=(const end_marker &) const noexcept { return m_action != nullptr; }

            bool operator==(const iterator & i_source) const noexcept { return m_action == i_source.m_action; }

            bool operator!=(const iterator & i_source) const noexcept { return m_action != i_source.m_action; }

            const char * get_string_value();

        private:
            friend class content;
            void next_base() noexcept;

            raw_ptr invoke(const array_view<const raw_ptr> & i_arguments);

        private:
            void * m_subobject = nullptr;
            const class_type * m_class = nullptr;
            const action * m_action = nullptr;
            size_t m_base_index = 0;
            raw_ptr const m_target;
            dyn_value m_dyn_value;
            std::vector<char> m_char_buffer;
        };

        iterator begin() const noexcept { return iterator(m_target); }

        end_marker end() const noexcept { return end_marker{}; }

    private:
        raw_ptr const m_target;
    };

    inline property_inspector inspect_properties(const raw_ptr & i_target) { return property_inspector(i_target); }

    inline action_inspector inspect_actions(const raw_ptr & i_target) { return action_inspector(i_target); }

} // namespace ediacaran
