#pragma once
#include <ediacaran/reflection/qualified_type_ptr.h>
#include <ediacaran/reflection/type.h>
#include <utility>

namespace ediacaran
{
    class raw_ptr
    {
      public:
        raw_ptr() noexcept = default;

        raw_ptr(const raw_ptr & i_source) noexcept = default;

        raw_ptr(void * i_object, const qualified_type_ptr & i_type) noexcept : m_object(i_object), m_type(i_type) {}

        template <typename TYPE> raw_ptr(TYPE * i_ptr) : m_object(i_ptr), m_type(get_qualified_type<TYPE>()) {}

        raw_ptr(raw_ptr && i_source) noexcept : m_object(i_source.m_object), m_type(i_source.m_type)
        {
            i_source.m_object = nullptr;
            i_source.m_type = qualified_type_ptr{};
        }

        raw_ptr & operator=(const raw_ptr & i_source) noexcept = default;

        raw_ptr & operator=(raw_ptr && i_source) noexcept
        {
            m_object = i_source.m_object;
            m_type = i_source.m_type;
            i_source.m_object = nullptr;
            i_source.m_type = qualified_type_ptr{};
            return *this;
        }

        friend void swap(raw_ptr & i_first, raw_ptr & i_second) noexcept
        {
            std::swap(i_first.m_object, i_second.m_object);
            std::swap(i_first.m_type, i_second.m_type);
        }

        bool empty() const noexcept { return m_object != nullptr; }

        bool operator==(const raw_ptr & i_source) const noexcept { return m_object == i_source.m_object; }

        bool operator!=(const raw_ptr & i_source) const noexcept { return !operator==(i_source); }

        const qualified_type_ptr & type() const noexcept { return m_type; }

        void * object() const noexcept { return m_object; }

        raw_ptr full_indirection() const noexcept
        {
            auto const indirection_levels = m_type.indirection_levels();

            auto object = m_object;
            for (size_t level = 0; level < indirection_levels; level++)
                object = *static_cast<void **>(object);

            return raw_ptr(
              object, qualified_type_ptr(m_type.final_type(), 0, m_type.is_const(indirection_levels) ? 1 : 0,
                        m_type.is_volatile(indirection_levels) ? 1 : 0));
        }

      private:
        void * m_object{nullptr};
        qualified_type_ptr m_type;
    };

} // namespace ediacaran