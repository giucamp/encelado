
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <ediacaran/reflection/class_type.h>
#include <ediacaran/utils/dyn_value.h>
#include <vector>

namespace edi
{
    class property_inspector;
    class function_inspector;

    property_inspector inspect_properties(const raw_ptr & i_target);

    function_inspector inspect_functions(const raw_ptr & i_target);

    dyn_value get_property_value(const raw_ptr & i_target, char_reader & i_property_name_source);

    dyn_value get_property_value(const raw_ptr & i_target, const string_view & i_property_name);

    void set_property_value(
      const raw_ptr & i_target, char_reader & i_property_name_source, const raw_ptr & i_value);

    void set_property_value(
      const raw_ptr & i_target, const string_view & i_property_name, const raw_ptr & i_value);

    void set_property_value(
      const raw_ptr & i_target, char_reader & i_property_name_source, char_reader & i_value_source);

    void set_property_value(
      const raw_ptr & i_target, const string_view & i_property_name, char_reader & i_value_source);

    void set_property_value(
      const raw_ptr &     i_target,
      char_reader &       i_property_name_source,
      const string_view & i_value_source);

    void set_property_value(
      const raw_ptr &     i_target,
      const string_view & i_property_name,
      const string_view & i_value_source);

    // expects "function(par1, par2, ...)"
    dyn_value invoke_function(const raw_ptr & i_target, char_reader & i_function_and_arguments);

    // expects "function(par1, par2, ...)"
    dyn_value
      invoke_function(const raw_ptr & i_target, const string_view & i_function_and_arguments);

    class property_inspector
    {
      public:
        property_inspector(const raw_ptr & i_target) noexcept
            : m_target(i_target.full_indirection())
        {
        }

        class iterator;

        class content
        {
          public:
            content(class iterator & m_parent) noexcept : m_parent(m_parent) {}

            string_view name() const noexcept { return m_parent.m_property->name(); }

            const qualified_type_ptr & qualified_type() const noexcept
            {
                return m_parent.m_property->qualified_type();
            }

            bool is_settable() const { return m_parent.m_property->is_settable(); }

            class_type const & owning_class() const noexcept { return *m_parent.m_class; }

            class property const & property() const noexcept { return *m_parent.m_property; }

            raw_ptr get_value() const { return m_parent.get_prop_value(); }

            void set_value(const raw_ptr & i_value) const { m_parent.set_prop_value(i_value); }

            void set_value(char_reader & i_source) const { m_parent.set_prop_value(i_source); }

            void set_value(const string_view & i_source) const
            {
                m_parent.set_prop_value(i_source);
            }

          private:
            class iterator & m_parent;
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

            bool operator==(const end_marker_t &) const noexcept { return m_property == nullptr; }

            bool operator!=(const end_marker_t &) const noexcept { return m_property != nullptr; }

            bool operator==(const iterator & i_source) const noexcept
            {
                return m_property == i_source.m_property;
            }

            bool operator!=(const iterator & i_source) const noexcept
            {
                return m_property != i_source.m_property;
            }

          private:
            friend class content;
            void    next_base() noexcept;
            raw_ptr get_prop_value();
            void    set_prop_value(const raw_ptr & i_value);
            void    set_prop_value(char_reader & i_source);
            void    set_prop_value(const string_view & i_source);

          private:
            void *             m_subobject  = nullptr;
            const class_type * m_class      = nullptr;
            const property *   m_property   = nullptr;
            size_t             m_base_index = 0;
            raw_ptr const      m_target;
            dyn_value          m_dyn_value;
        };

        iterator begin() const noexcept { return iterator(m_target); }

        end_marker_t end() const noexcept { return end_marker; }

      private:
        raw_ptr const m_target;
    };

    class function_inspector
    {
      public:
        function_inspector(const raw_ptr & i_target) noexcept
            : m_target(i_target.full_indirection())
        {
        }

        class iterator;

        class content
        {
          public:
            content(class iterator & m_parent) noexcept : m_parent(m_parent) {}

            string_view name() const noexcept { return m_parent.m_function->name(); }

            const qualified_type_ptr qualified_return_type() const noexcept
            {
                return m_parent.m_function->qualified_return_type();
            }

            array_view<const parameter> const & parameters() const noexcept
            {
                return m_parent.m_function->parameters();
            }

            class_type const & owning_class() const noexcept { return *m_parent.m_class; }

            class function const & function() const noexcept { return *m_parent.m_function; }

            raw_ptr invoke(const array_view<const raw_ptr> & i_arguments) const
            {
                return m_parent.invoke(i_arguments);
            }

            raw_ptr invoke(const array_view<string_view> & i_arguments) const
            {
                return m_parent.invoke(i_arguments);
            }

            raw_ptr invoke(char_reader & i_arguments) const { return m_parent.invoke(i_arguments); }

            raw_ptr invoke(const string_view & i_arguments) const
            {
                char_reader arguments_source(i_arguments);
                return m_parent.invoke(arguments_source);
            }

          private:
            class iterator & m_parent;
        };

        class iterator
        {
          public:
            iterator(const raw_ptr & i_target) noexcept;

            content operator*() noexcept { return content(*this); }

            iterator & operator++() noexcept
            {
                m_function++;
                if (m_function == m_class->functions().end())
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

            bool operator==(const end_marker_t &) const noexcept { return m_function == nullptr; }

            bool operator!=(const end_marker_t &) const noexcept { return m_function != nullptr; }

            bool operator==(const iterator & i_source) const noexcept
            {
                return m_function == i_source.m_function;
            }

            bool operator!=(const iterator & i_source) const noexcept
            {
                return m_function != i_source.m_function;
            }

          private:
            friend class content;
            void next_base() noexcept;

            raw_ptr invoke(const array_view<const raw_ptr> & i_arguments);
            raw_ptr invoke(const array_view<string_view> & i_arguments);
            raw_ptr invoke(char_reader & i_arguments_source);

          private:
            void *             m_subobject  = nullptr;
            const class_type * m_class      = nullptr;
            const function *   m_function   = nullptr;
            size_t             m_base_index = 0;
            raw_ptr const      m_target;
            dyn_value          m_dyn_value;
            std::vector<char>  m_char_buffer;
        };

        iterator begin() const noexcept { return iterator(m_target); }

        end_marker_t end() const noexcept { return end_marker; }

      private:
        raw_ptr const m_target;
    };

} // namespace edi
