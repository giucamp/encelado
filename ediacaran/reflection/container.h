#pragma once

#include "ediacaran/reflection/qualified_type_ptr.h"

namespace ediacaran
{


    class container
    {
      public:
        enum class capabilities
        {
            none          = 0,
            heterogeneous = 1 << 0,
            contigous     = 1 << 1,
        };

        constexpr friend capabilities
          operator|(capabilities i_first, capabilities i_seconds) noexcept
        {
            using underlying_type = std::underlying_type<enum capabilities>::type;
            return static_cast<enum capabilities>(
              static_cast<underlying_type>(i_first) | static_cast<underlying_type>(i_seconds));
        }

        constexpr friend capabilities
          operator&(capabilities i_first, capabilities i_seconds) noexcept
        {
            using underlying_type = std::underlying_type<enum capabilities>::type;
            return static_cast<enum capabilities>(
              static_cast<underlying_type>(i_first) & static_cast<underlying_type>(i_seconds));
        }

        using index                                = uint64_t;
        using signed_index                         = int64_t;
        constexpr static size_t iterator_alignment = alignof(void *);

        using construct_iterator_function_ptr =
          void (*)(void * i_container, void * i_iterator_dest, index i_start_index);
        using destroy_iterator_function_ptr = void (*)(void * i_iterator_dest) noexcept;

        using iterator_move_and_get_function_ptr = void (*)(
          void *               i_iterator,
          signed_index         i_index_offset,
          qualified_type_ptr * o_element_type,
          void **              o_elements,
          index *              o_count);

        constexpr container() = default;

        constexpr container(
          capabilities                       i_capabilities,
          qualified_type_ptr                 i_elements_type,
          size_t                             i_iterator_size,
          construct_iterator_function_ptr    i_construct_iterator,
          destroy_iterator_function_ptr      i_destroy_iterator_function,
          iterator_move_and_get_function_ptr i_iterator_move_and_get) noexcept
            : m_capabilities(i_capabilities), m_elements_type(i_elements_type),
              m_iterator_size(i_iterator_size), m_construct_iterator(i_construct_iterator),
              m_destroy_iterator_function(i_destroy_iterator_function),
              m_iterator_move_and_get(i_iterator_move_and_get)
        {
        }

        capabilities                    capabilities() const noexcept { return m_capabilities; }
        qualified_type_ptr              elements_type() const noexcept { return m_elements_type; }
        size_t                          iterator_size() const noexcept { return m_iterator_size; }
        construct_iterator_function_ptr construct_iterator() const noexcept
        {
            return m_construct_iterator;
        }
        destroy_iterator_function_ptr destroy_iterator_function() const noexcept
        {
            return m_destroy_iterator_function;
        }
        iterator_move_and_get_function_ptr iterator_move_and_get() const noexcept
        {
            return m_iterator_move_and_get;
        }

      private:
        enum capabilities const m_capabilities
        {
            capabilities::none
        };
        qualified_type_ptr const                 m_elements_type;
        size_t const                             m_iterator_size{};
        construct_iterator_function_ptr const    m_construct_iterator{};
        destroy_iterator_function_ptr const      m_destroy_iterator_function{};
        iterator_move_and_get_function_ptr const m_iterator_move_and_get{};
    };

} // namespace ediacaran
