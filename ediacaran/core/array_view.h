#pragma once
#include <initializer_list>
#include <cstddef>

namespace ediacaran
{
    template <typename TYPE> class array_view
    {
      public:

        constexpr array_view() noexcept : m_objects(nullptr), m_size(0) {}

        constexpr array_view(TYPE * i_objects, size_t i_size) noexcept : m_objects(i_objects), m_size(i_size) {}

        template <size_t SIZE> constexpr array_view(TYPE (&i_objects)[SIZE]) noexcept : m_objects(i_objects), m_size(SIZE) {}

        constexpr array_view(std::initializer_list<TYPE> i_initializer_list) noexcept
            : m_objects(i_initializer_list.begin()), m_size(i_initializer_list.end() - i_initializer_list.begin())
        {
        }

        constexpr TYPE * data() const noexcept { return m_objects; }

        constexpr TYPE * begin() const noexcept { return m_objects; }

        constexpr TYPE * end() const noexcept { return m_objects + m_size; }

        constexpr TYPE * cbegin() const noexcept { return m_objects; }

        constexpr TYPE * cend() const noexcept { return m_objects + m_size; }

        constexpr TYPE & operator[](size_t i_index) noexcept
        {
            assert(i_index < m_size);
            return m_objects[i_index];
        }

        constexpr const TYPE & operator[](size_t i_index) const noexcept
        {
            assert(i_index < m_size);
            return m_objects[i_index];
        }

        constexpr size_t size() const noexcept { return m_size; }

      private:
        TYPE * m_objects;
        size_t m_size;
    };
}