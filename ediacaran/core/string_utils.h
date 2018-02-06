
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
    class split
    {

      private:
        string_view m_source;
        char        m_separator{};
      public:
        constexpr split() noexcept = default;

        constexpr split(const string_view & i_source, char i_separator) noexcept
            : m_source(i_source), m_separator(i_separator)
        {
        }

        constexpr split(const split &) noexcept = default;
        constexpr split(split &&) noexcept      = default;

        constexpr split & operator=(const split &) noexcept = default;
        constexpr split & operator=(split &&) noexcept = default;

        struct end_marker
        {
        };

        class const_iterator
        {
          public:
            using iterator_category = std::input_iterator_tag;

            constexpr const_iterator() noexcept                       = default;

            constexpr const_iterator(const string_view & i_source, char i_separator)
                : m_end_of_string(i_source.data() + i_source.size()), m_separator(i_separator)
            {
                m_token = get_next(i_source.data());
            }

            constexpr const_iterator(const const_iterator &) noexcept = default;
            constexpr const_iterator(const_iterator &&) noexcept      = default;

            constexpr const_iterator & operator=(const const_iterator &) noexcept = default;
            constexpr const_iterator & operator=(const_iterator &&) noexcept = default;

            constexpr const string_view & operator*() const noexcept { return m_token; }

            constexpr const_iterator & operator++() noexcept
            {
                m_token = get_next(m_token.data() + m_token.size() + 1);
                return *this;
            }

            constexpr const_iterator operator++(int)noexcept
            {
                auto const copy{*this};
                m_token = get_next(m_token.data() + m_token.size() + 1);
                return copy;
            }

            constexpr bool operator==(end_marker) noexcept { return is_over(); }

            constexpr bool operator!=(end_marker) noexcept { return !is_over(); }

          private:
            constexpr string_view get_next(const char * i_from) noexcept
            {
                auto curr = i_from;
                do
                {
                    if (curr >= m_end_of_string)
                    {
                        return {};
                    }
                } while (*++curr != m_separator);
                EDIACARAN_ASSERT(curr >= i_from);
                return {i_from, static_cast<string_view::size_type>(curr - i_from)};
            }

            constexpr bool is_over() const noexcept { return m_token.data() == nullptr; }

          private:
            string_view  m_token;
            const char * m_end_of_string{};
            char         m_separator{};
        };

        using iterator = const_iterator;

        constexpr const_iterator cbegin() const noexcept
        {
            return const_iterator(m_source, m_separator);
        }

        constexpr end_marker cend() const noexcept { return end_marker{}; }

        constexpr iterator begin() const noexcept { return iterator(m_source, m_separator); }

        constexpr end_marker end() const noexcept { return end_marker{}; }
    };

} // namespace edi
