#pragma once
#include <cstddef>
#include <ediacaran/core/ediacaran_common.h>
#include <initializer_list>
#include <array>
#include <utility>
#include <type_traits>

namespace ediacaran
{
    template <typename... ARG_TYPES>
        constexpr auto make_array(ARG_TYPES &&... i_elements)
    {
        return std::array<std::common_type_t<ARG_TYPES...>, sizeof...(ARG_TYPES)>{{ std::forward<ARG_TYPES>(i_elements)... }};
    }

    template <typename TYPE> class array_view
    {
      public:
        constexpr array_view() noexcept : m_objects(nullptr), m_size(0) {}

        constexpr array_view(TYPE * i_objects, size_t i_size) noexcept : m_objects(i_objects), m_size(i_size) {}

        template <size_t SIZE>
        constexpr array_view(TYPE (&i_objects)[SIZE]) noexcept : m_objects(i_objects), m_size(SIZE)
        {
        }

        template <typename SOURCE_TYPE, size_t SIZE>
        constexpr array_view(const std::array<SOURCE_TYPE, SIZE> & i_objects) noexcept : m_objects(i_objects.data()), m_size(SIZE)
        {
        }

        constexpr array_view(std::initializer_list<TYPE> i_initializer_list) noexcept
            : m_objects(i_initializer_list.begin()), m_size(i_initializer_list.end() - i_initializer_list.begin())
        {
        }

        constexpr TYPE * data() const noexcept { return m_objects; }

        constexpr TYPE * begin() const noexcept { return m_objects; }

        constexpr TYPE * end() const noexcept { return m_objects + m_size; }

        constexpr TYPE * cbegin() const noexcept { return m_objects; }

        constexpr TYPE * cend() const noexcept { return m_objects + m_size; }

        constexpr bool empty() const noexcept { return m_size == 0; }

        constexpr TYPE & operator[](size_t i_index) noexcept
        {
            EDIACARAN_ASSERT(i_index < m_size);
            return m_objects[i_index];
        }

        constexpr const TYPE & operator[](size_t i_index) const noexcept
        {
            EDIACARAN_ASSERT(i_index < m_size);
            return m_objects[i_index];
        }

        constexpr size_t size() const noexcept { return m_size; }

      private:
        TYPE * m_objects;
        size_t m_size;
    };
}
