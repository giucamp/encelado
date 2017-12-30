#pragma once
#include <ediacaran/reflection/qualified_type_ptr.h>
#include <ediacaran/reflection/type.h>
#include <ediacaran/utils/raw_ptr.h>
#include <utility>

namespace ediacaran
{
    class dyn_value
    {
      public:
        dyn_value() noexcept = default;

        dyn_value(const dyn_value & i_source) : dyn_value(raw_ptr(i_source)) {}

        dyn_value(const raw_ptr & i_source);

        operator raw_ptr() const noexcept { return raw_ptr(m_object, m_qualified_type); }

        dyn_value(dyn_value && i_source) noexcept : m_object(i_source.m_object), m_qualified_type(i_source.m_qualified_type)
        {
            i_source.m_object = nullptr;
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

        void * uninitialized_allocate(const qualified_type_ptr & i_type);

        void uninitialized_deallocate() noexcept;

        template <typename CALLABLE>
        void * manual_construct(const qualified_type_ptr & i_type, CALLABLE && i_constructor)
        {
            try
            {
                uninitialized_allocate(i_type);
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

        void * object() noexcept { return m_object; }

      private:
        void destroy() noexcept
        {
            m_qualified_type.primary_type()->destroy(m_object);
            uninitialized_deallocate();
        }

      private:
        void * m_object{nullptr};
        qualified_type_ptr m_qualified_type;
    };

} // namespace ediacaran