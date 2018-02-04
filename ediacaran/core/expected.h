#pragma once
#include <ediacaran/core/ediacaran_common.h>
#include <type_traits>
#include <utility>
#include <variant>

namespace ediacaran
{
    struct unexpected_t
    {
    };

    template <typename ERROR> class bad_expected_access : public std::exception
    {
      public:
        virtual char const * what() const { return "value not present"; }

      private:
        ERROR m_error;
    };

    template <typename VALUE, typename ERROR> class[[nodiscard]] expected
    {
      public:
        template <
          typename... PARAMS,
          std::enable_if_t<std::is_constructible_v<VALUE, PARAMS...>> * = nullptr>
        constexpr expected(PARAMS && ... i_params)
            : m_value(std::in_place_index_t<0>{}, std::forward<PARAMS>(i_params)...)
        {
        }

        template <
          typename... PARAMS,
          std::enable_if_t<std::is_constructible_v<ERROR, PARAMS...>> * = nullptr>
        constexpr expected(PARAMS && ... i_params)
            : m_value(std::in_place_index_t<1>{}, std::forward<PARAMS>(i_params)...)
        {
        }

        template <typename VAL = VALUE, std::enable_if_t<std::is_void_v<VAL>> * = nullptr>
        constexpr expected() : m_value(std::in_place_index_t<0>{})
        {
        }

        template <
          typename... PARAMS,
          std::enable_if_t<std::is_constructible_v<VALUE, PARAMS...>> * = nullptr>
        constexpr expected(std::in_place_t, PARAMS && ... i_params)
            : m_value(std::in_place_index_t<0>{}, std::forward<PARAMS>(i_params)...)
        {
        }

        template <
          typename... PARAMS,
          std::enable_if_t<std::is_constructible_v<ERROR, PARAMS...>> * = nullptr>
        constexpr expected(unexpected_t, PARAMS && ... i_params)
            : m_value(std::in_place_index_t<0>{}, std::forward<PARAMS>(i_params)...)
        {
        }

        constexpr bool has_value() const noexcept { return m_value.index() == 0; }

        constexpr bool has_error() const noexcept { return m_value.index() == 1; }

        constexpr operator bool() const noexcept { return has_value(); }

        template <typename VAL = VALUE, std::enable_if_t<!std::is_void_v<VAL>> * = nullptr>
        constexpr const VAL & value() const
        {
            if (has_value())
                return std::get<0>(m_value);
            else
                throw_error();
        }

        constexpr void on_error_except() const
        {
            if (has_error())
                throw_error();
        }

        constexpr const ERROR & error() const noexcept
        {
            EDIACARAN_ASSERT(!has_value());
            return std::get<1>(m_value);
        }

      private:
        [[noreturn]] constexpr void throw_error() const { throw std::get<1>(m_value); }

      private:
        std::variant<std::conditional_t<std::is_void_v<VALUE>, bool, VALUE>, ERROR> m_value;
    };
}
