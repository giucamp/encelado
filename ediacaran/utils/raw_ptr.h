#pragma once
#include <ediacaran/core/char_writer.h>
#include <ediacaran/core/string_builder.h>
#include <ediacaran/reflection/reflection.h>
#include <string>
#include <utility>

namespace ediacaran
{
    class raw_ptr
    {
      public:
        raw_ptr() noexcept = default;

        raw_ptr(const raw_ptr & i_source) noexcept = default;

        raw_ptr(void * i_object, const qualified_type_ptr & i_type) noexcept
            : m_object(i_object), m_qualified_type(i_type)
        {
        }

        template <typename TYPE>
        explicit raw_ptr(TYPE * i_ptr)
            : m_object(i_ptr), m_qualified_type(get_qualified_type<TYPE>())
        {
        }

        raw_ptr(raw_ptr && i_source) noexcept
            : m_object(i_source.m_object), m_qualified_type(i_source.m_qualified_type)
        {
            i_source.m_object         = nullptr;
            i_source.m_qualified_type = qualified_type_ptr{};
        }

        raw_ptr & operator=(const raw_ptr & i_source) noexcept = default;

        raw_ptr & operator=(raw_ptr && i_source) noexcept
        {
            m_object                  = i_source.m_object;
            m_qualified_type          = i_source.m_qualified_type;
            i_source.m_object         = nullptr;
            i_source.m_qualified_type = qualified_type_ptr{};
            return *this;
        }

        friend void swap(raw_ptr & i_first, raw_ptr & i_second) noexcept
        {
            std::swap(i_first.m_object, i_second.m_object);
            std::swap(i_first.m_qualified_type, i_second.m_qualified_type);
        }

        bool empty() const noexcept { return m_object == nullptr; }

        bool operator==(const raw_ptr & i_source) const noexcept
        {
            return m_object == i_source.m_object;
        }

        bool operator!=(const raw_ptr & i_source) const noexcept { return !operator==(i_source); }

        const qualified_type_ptr & qualified_type() const noexcept { return m_qualified_type; }

        const void * object() const noexcept { return m_object; }

        void * editable_object() const
        {
            if (m_qualified_type.is_const(0))
                except<constness_violation>("constness_violation");
            return m_object;
        }

        raw_ptr full_indirection() const;

        raw_ptr try_full_indirection() const noexcept;

        void to_string(char_writer & o_dest) const noexcept;

        void to_string(std::string & o_dest) const;

        std::string to_string() const
        {
            std::string dest;
            to_string(dest);
            return dest;
        }

      private:
        void *             m_object{nullptr};
        qualified_type_ptr m_qualified_type;
    };

    inline char_writer & operator<<(char_writer & o_dest, const raw_ptr & i_ptr)
    {
        i_ptr.to_string(o_dest);
        return o_dest;
    }

    inline string_builder & operator<<(string_builder & o_dest, const raw_ptr & i_ptr)
    {
        o_dest.custom_write([&i_ptr](char_writer & o_dest) { o_dest << i_ptr; });
        return o_dest;
    }

} // namespace ediacaran
