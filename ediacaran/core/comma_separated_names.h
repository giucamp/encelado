
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2016-2017.

#pragma once
#include "ediacaran/core/char_reader.h"
#include <iterator>

namespace ediacaran
{
    class comma_separated_names
    {
    public:

        constexpr comma_separated_names(const char * i_string) noexcept
            : m_string(i_string) {}

        class const_iterator_end_marker
        {
        };

        class const_iterator
        {
        public:

            using difference_type = std::ptrdiff_t;
            using value_type = string_view;
            using pointer = string_view*;
            using reference	= string_view &;
            using iterator_category	= std::input_iterator_tag;

            constexpr const_iterator(const comma_separated_names & i_parent) noexcept
                : m_reader(i_parent.m_string)
            {
                try_accept(spaces, m_reader);
                m_current = try_parse_identifier(m_reader);
            }

            constexpr const string_view & operator * () const noexcept
            {
                return m_current;
            }

            constexpr bool operator == (const_iterator_end_marker) const noexcept
            {
                return m_current.empty();
            }

            constexpr bool operator != (const_iterator_end_marker i_marker) const noexcept
            {
                return !operator == (i_marker);
            }

            constexpr const_iterator & operator ++ () noexcept
            {
                try_accept(spaces, m_reader);
                if (try_accept(',', m_reader))
                {
                    try_accept(spaces, m_reader);
                    m_current = try_parse_identifier(m_reader);
                }
                else
                {
                    m_current = string_view{};
                }
                return *this;
            }

            constexpr const_iterator operator ++ (int) noexcept
            {
                auto const copy{*this};
                operator ++ ();
                return copy;
            }

        private:
            char_reader m_reader;
            string_view m_current;
        };

        constexpr const_iterator begin() const noexcept
        {
            return const_iterator(*this);
        }

        constexpr const_iterator_end_marker end() const noexcept
        {
            return const_iterator_end_marker{};
        }

        constexpr const_iterator cbegin() const noexcept
        {
            return const_iterator(*this);
        }

        constexpr const_iterator_end_marker cend() const noexcept
        {
            return const_iterator_end_marker{};
        }

    private:
        const char * m_string;
    };

} // namespace ediacaran
