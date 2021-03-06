
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "ediacaran/core/char_reader.h"
#include <iterator>
#include <stdexcept>

namespace edi
{
    class comma_separated_names
    {
      public:
        constexpr comma_separated_names(const char * i_string) noexcept : m_string(i_string) {}

        class const_iterator
        {
          public:
            using difference_type   = std::ptrdiff_t;
            using value_type        = string_view;
            using pointer           = string_view *;
            using reference         = string_view &;
            using iterator_category = std::input_iterator_tag;

            constexpr const_iterator(const comma_separated_names & i_parent)
                : m_reader(i_parent.m_string)
            {
                (void)accept(spaces, m_reader);
                m_current = try_parse_identifier(m_reader);
                check_for_end();
            }

            constexpr const string_view & operator*() const noexcept { return m_current; }

            constexpr bool operator==(end_marker_t) const noexcept { return m_current.empty(); }

            constexpr bool operator!=(end_marker_t i_marker) const noexcept
            {
                return !operator==(i_marker);
            }

            constexpr const_iterator & operator++()
            {
                (void)accept(spaces, m_reader);
                if (accept(',', m_reader).has_value())
                {
                    (void)accept(spaces, m_reader);
                    m_current = try_parse_identifier(m_reader);
                }
                else
                {
                    m_current = string_view{};
                }
                check_for_end();
                return *this;
            }

            constexpr const_iterator operator++(int)
            {
                auto const copy{*this};
                           operator++();
                return copy;
            }

          private:
            constexpr void check_for_end() const
            {
                if (m_current.empty() && m_reader.remaining_chars() != 0)
                    except<std::logic_error>("Unrecognized chars in comma separated list");
            }

          private:
            char_reader m_reader;
            string_view m_current;
        };

        constexpr const_iterator begin() const { return const_iterator(*this); }

        constexpr end_marker_t end() const noexcept { return end_marker; }

        constexpr const_iterator cbegin() const { return const_iterator(*this); }

        constexpr end_marker_t cend() const noexcept { return end_marker; }

      private:
        const char * m_string;
    };

} // namespace edi
