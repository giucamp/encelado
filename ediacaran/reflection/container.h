
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "ediacaran/reflection/qualified_type_ptr.h"

namespace edi
{
    class container
    {
      public:
        enum class capability
        {
            none          = 0,
            heterogeneous = 1 << 0,
            contigous     = 1 << 1,
        };

        constexpr friend capability operator|(capability i_first, capability i_seconds) noexcept
        {
            using underlying_type = std::underlying_type<capability>::type;
            return static_cast<capability>(
              static_cast<underlying_type>(i_first) | static_cast<underlying_type>(i_seconds));
        }

        constexpr friend capability operator&(capability i_first, capability i_seconds) noexcept
        {
            using underlying_type = std::underlying_type<capability>::type;
            return static_cast<capability>(
              static_cast<underlying_type>(i_first) & static_cast<underlying_type>(i_seconds));
        }

        using index                                = uint64_t;
        constexpr static size_t iterator_alignment = alignof(void *);

        struct segment
        {
            qualified_type_ptr element_type;
            void *             m_elements{};
            index              m_element_count{};
        };

        using construct_iterator_function_ptr =
          segment (*)(void * i_container, void * i_iterator_dest);

        using next_segment_function_ptr = segment (*)(void * i_iterator);

        using destroy_iterator_function_ptr = void (*)(void * i_iterator) noexcept;

        constexpr container(
          capability                      i_capabilities,
          qualified_type_ptr              i_elements_type,
          size_t                          i_iterator_size,
          construct_iterator_function_ptr i_construct_iterator,
          next_segment_function_ptr       i_next_segment,
          destroy_iterator_function_ptr   i_destroy_iterator_function) noexcept
            : m_capabilities(i_capabilities), m_elements_type(i_elements_type),
              m_iterator_size(i_iterator_size), m_construct_iterator(i_construct_iterator),
              m_next_segment(i_next_segment),
              m_destroy_iterator_function(i_destroy_iterator_function)

        {
            EDIACARAN_ASSERT(i_construct_iterator != nullptr);
            EDIACARAN_ASSERT(i_next_segment != nullptr);
            EDIACARAN_ASSERT(i_destroy_iterator_function != nullptr);
        }

        capability                      capabilities() const noexcept { return m_capabilities; }
        qualified_type_ptr              elements_type() const noexcept { return m_elements_type; }
        size_t                          iterator_size() const noexcept { return m_iterator_size; }
        construct_iterator_function_ptr construct_iterator() const noexcept
        {
            return m_construct_iterator;
        }
        next_segment_function_ptr     next_segment() const noexcept { return m_next_segment; }
        destroy_iterator_function_ptr destroy_iterator_function() const noexcept
        {
            return m_destroy_iterator_function;
        }

      private:
        capability const                      m_capabilities{capability::none};
        qualified_type_ptr const              m_elements_type{};
        size_t const                          m_iterator_size{};
        construct_iterator_function_ptr const m_construct_iterator{};
        next_segment_function_ptr const       m_next_segment{};
        destroy_iterator_function_ptr const   m_destroy_iterator_function{};
    };

} // namespace edi
