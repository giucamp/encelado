#pragma once
#include <ediacaran/core/ediacaran_common.h>
#include <iterator>
#include <stdexcept>

namespace ediacaran
{
    template <typename TYPE, size_t SIZE> struct array
    {
        using value_type             = TYPE;
        using size_type              = size_t;
        using difference_type        = ptrdiff_t;
        using reference              = TYPE &;
        using const_reference        = const TYPE &;
        using pointer                = TYPE *;
        using const_pointer          = const TYPE *;
        using iterator               = TYPE *;
        using const_iterator         = const TYPE *;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        constexpr reference at(size_type i_index)
        {
            if (i_index >= SIZE)
                throw std::out_of_range{"array: out of range"};
            return m_array[i_index];
        }

        constexpr const_reference at(size_type i_index) const
        {
            if (i_index >= SIZE)
                throw std::out_of_range{"array: out of range"};
            return m_array[i_index];
        }

        constexpr reference operator[](size_type i_index) noexcept
        {
            EDIACARAN_ASSERT(i_index < SIZE);
            return m_array[i_index];
        }

        constexpr const_reference operator[](size_type i_index) const noexcept
        {
            EDIACARAN_ASSERT(i_index < SIZE);
            return m_array[i_index];
        }

        constexpr reference front() noexcept { return m_array[0]; }

        constexpr const_reference front() const noexcept { return m_array[0]; }

        constexpr reference back() noexcept { return m_array[SIZE - 1]; }

        constexpr const_reference back() const noexcept { return m_array[SIZE - 1]; }

        constexpr pointer data() noexcept { return m_array; }

        constexpr const_pointer data() const noexcept { return m_array; }

        constexpr size_type size() const noexcept { return SIZE; }

        constexpr iterator begin() noexcept { return m_array; }
        constexpr iterator end() noexcept { return m_array + SIZE; }

        constexpr const_iterator begin() const noexcept { return m_array; }
        constexpr const_iterator end() const noexcept { return m_array + SIZE; }

        constexpr const_iterator cbegin() const noexcept { return m_array; }
        constexpr const_iterator cend() const noexcept { return m_array + SIZE; }

        TYPE m_array[SIZE];
    };

    template <typename TYPE> struct array<TYPE, 0>
    {
        using value_type             = TYPE;
        using size_type              = size_t;
        using difference_type        = ptrdiff_t;
        using reference              = TYPE &;
        using const_reference        = const TYPE &;
        using pointer                = TYPE *;
        using const_pointer          = const TYPE *;
        using iterator               = TYPE *;
        using const_iterator         = const TYPE *;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        [[noreturn]] constexpr reference at(size_type)
        {
            throw std::out_of_range{"array: out of range"};
        }

        [[noreturn]] constexpr const_reference at(size_type) const
        {
            throw std::out_of_range{"array: out of range"};
        }

        [[noreturn]] constexpr reference operator[](size_type) noexcept { EDIACARAN_ASSERT(false); }

        [[noreturn]] constexpr const_reference operator[](size_type) const noexcept
        {
            EDIACARAN_ASSERT(false);
        }

        [[noreturn]] constexpr reference front() noexcept { EDIACARAN_ASSERT(false); }

        [[noreturn]] constexpr const_reference front() const noexcept { EDIACARAN_ASSERT(false); }

        [[noreturn]] constexpr reference back() noexcept { EDIACARAN_ASSERT(false); }

        [[noreturn]] constexpr const_reference back() const noexcept { EDIACARAN_ASSERT(false); }

        constexpr pointer data() noexcept { return nullptr; }

        constexpr const_pointer data() const noexcept { return nullptr; }

        constexpr size_type size() const noexcept { return 0; }

        constexpr iterator begin() noexcept { return nullptr; }
        constexpr iterator end() noexcept { return nullptr; }

        constexpr const_iterator begin() const noexcept { return nullptr; }
        constexpr const_iterator end() const noexcept { return nullptr; }

        constexpr const_iterator cbegin() const noexcept { return nullptr; }
        constexpr const_iterator cend() const noexcept { return nullptr; }
    };

    template <typename... ARG_TYPES> constexpr auto make_array(ARG_TYPES &&... i_elements)
    {
        return array<std::common_type_t<ARG_TYPES...>, sizeof...(ARG_TYPES)>{
          {std::forward<ARG_TYPES>(i_elements)...}};
    }
}
