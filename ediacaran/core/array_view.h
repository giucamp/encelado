#pragma once
#include <initializer_list>
#include <cstddef>

namespace ediacaran
{
    template <typename TYPE> class array_view
    {
      public:
        array_view() : m_objects(nullptr), m_size(0) {}

        array_view(TYPE * i_objects, size_t i_size) : m_objects(i_objects), m_size(i_size) {}

        template <size_t SIZE> array_view(TYPE (&i_objects)[SIZE]) : m_objects(i_objects), m_size(SIZE) {}

        array_view(std::initializer_list<TYPE> i_initializer_list)
            : m_objects(i_initializer_list.begin()), m_size(i_initializer_list.end() - i_initializer_list.begin())
        {
        }

        TYPE * data() const { return m_objects; }

        TYPE * begin() const { return m_objects; }

        TYPE * end() const { return m_objects + m_size; }

        TYPE * cbegin() const { return m_objects; }

        TYPE * cend() const { return m_objects + m_size; }

        TYPE & operator[](size_t i_index)
        {
            assert(i_index < m_size);
            return m_objects[i_index];
        }

        const TYPE & operator[](size_t i_index) const
        {
            assert(i_index < m_size);
            return m_objects[i_index];
        }

        size_t size() const { return m_size; }

      private:
        TYPE * m_objects;
        size_t m_size;
    };
}