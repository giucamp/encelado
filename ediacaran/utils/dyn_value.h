#pragma once
#include <ediacaran/reflection/reflection.h>
#include <ediacaran/utils/raw_ptr.h>
#include <utility>

namespace ediacaran
{
    class dyn_value
    {
      public:
        dyn_value() noexcept = default;

        dyn_value(const dyn_value & i_source) : dyn_value(raw_ptr(i_source)) {}

        dyn_value(const qualified_type_ptr & i_qualified_type);

        dyn_value(const raw_ptr & i_source);

        dyn_value(dyn_value && i_source) noexcept
            : m_object(i_source.m_object), m_qualified_type(i_source.m_qualified_type)
        {
            i_source.m_object         = nullptr;
            i_source.m_qualified_type = qualified_type_ptr{};
        }

        dyn_value & operator=(const dyn_value & i_source)
        {
            dyn_value copy(i_source);
            swap(*this, copy);
            return *this;
        }

        dyn_value & operator=(dyn_value && i_source) noexcept
        {
            dyn_value copy(i_source);
            swap(*this, i_source);
            return *this;
        }

        void assign(const qualified_type_ptr & i_qualified_type);

        operator raw_ptr() const noexcept { return raw_ptr(m_object, m_qualified_type); }

        template <typename CALLABLE>
        void * manual_construct(const qualified_type_ptr & i_type, CALLABLE && i_constructor)
        {
            uninitialized_allocate(i_type);
            try
            {
                std::forward<CALLABLE>(i_constructor)(m_object);
            }
            catch (...)
            {
                uninitialized_deallocate();
                throw;
            }
            return m_object;
        }

        ~dyn_value()
        {
            if (m_object != nullptr)
                destroy();
        }

        friend void swap(dyn_value & i_first, dyn_value & i_second) noexcept
        {
            std::swap(i_first.m_object, i_second.m_object);
            std::swap(i_first.m_qualified_type, i_second.m_qualified_type);
        }

        bool empty() const noexcept { return m_object == nullptr; }

        bool operator==(const dyn_value & i_source) const;

        bool operator!=(const dyn_value & i_source) const { return !operator==(i_source); }

        const qualified_type_ptr & qualified_type() const noexcept { return m_qualified_type; }

        const void * object() const noexcept { return m_object; }

        void * edit_object() noexcept { return m_object; }

        void to_string(char_writer & o_dest) const noexcept
        {
            raw_ptr(m_object, m_qualified_type).to_string(o_dest);
        }

        void to_string(std::string & o_dest) const
        {
            raw_ptr(m_object, m_qualified_type).to_string(o_dest);
        }

        std::string to_string() const { return raw_ptr(m_object, m_qualified_type).to_string(); }

      private:
        void * uninitialized_allocate(const qualified_type_ptr & i_type);

        void uninitialized_deallocate() noexcept;

        void destroy() noexcept
        {
            m_qualified_type.primary_type()->destroy(m_object);
            uninitialized_deallocate();
        }

      private:
        void *             m_object{nullptr};
        qualified_type_ptr m_qualified_type;
    };

    inline char_writer & operator<<(char_writer & o_dest, const dyn_value & i_value)
    {
        static_cast<raw_ptr>(i_value).to_string(o_dest);
        return o_dest;
    }

    inline string_builder & operator<<(string_builder & o_dest, const dyn_value & i_value)
    {
        o_dest.custom_write([&i_value](char_writer & o_dest) { o_dest << i_value; });
        return o_dest;
    }

    dyn_value parse_value(const qualified_type_ptr & i_qualified_type, char_reader & i_source);
    dyn_value parse_value(const qualified_type_ptr & i_qualified_type, const string_view & i_source);

} // namespace ediacaran
