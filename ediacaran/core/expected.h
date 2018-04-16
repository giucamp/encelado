
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <ediacaran/core/ediacaran_common.h>
#include <memory>
#include <type_traits>

namespace edi
{
    struct suceess_t
    {
    };

    constexpr suceess_t suceess;

    struct unexpected_t
    {
    };

    constexpr unexpected_t unexpected;

    template <typename ERROR> class bad_expected_access : public std::exception
    {
      public:
        virtual char const * what() const noexcept { return "value not present"; }

      private:
        ERROR m_error;
    };

    namespace detail
    {
        template <typename VALUE, typename ERROR, bool TRIVIAL_DESTRUCTOR> struct ExpectedBase;

        template <typename VALUE, typename ERROR> struct ExpectedBase<VALUE, ERROR, false>
        {
            template <
              typename FIRST_PARAM,
              typename... PARAMS,
              std::enable_if_t<std::is_constructible_v<VALUE, FIRST_PARAM, PARAMS...>> * = nullptr>
            constexpr ExpectedBase(FIRST_PARAM && i_first_param, PARAMS &&... i_params)
                : m_value(
                    std::forward<FIRST_PARAM>(i_first_param), std::forward<PARAMS>(i_params)...),
                  m_has_value(true)
            {
                static_assert(
                  !std::is_constructible_v<ERROR, FIRST_PARAM, PARAMS...>,
                  "ambiguous construction: ERROR is constructible with the "
                  "same arguments");
            }

            template <
              typename FIRST_PARAM,
              typename... PARAMS,
              std::enable_if_t<std::is_constructible_v<ERROR, FIRST_PARAM, PARAMS...>> * = nullptr>
            constexpr ExpectedBase(FIRST_PARAM && i_first_param, PARAMS &&... i_params)
                : m_error(
                    std::forward<FIRST_PARAM>(i_first_param), std::forward<PARAMS>(i_params)...),
                  m_has_value(false)
            {
                static_assert(
                  !std::is_constructible_v<VALUE, FIRST_PARAM, PARAMS...>,
                  "ambiguous construction: VALUE is constructible with the "
                  "same arguments");
            }

            template <typename VAL = VALUE, std::enable_if_t<std::is_void_v<VAL>> * = nullptr>
            constexpr ExpectedBase() : m_value{}, m_has_value(true)
            {
            }

            template <typename... PARAMS>
            constexpr ExpectedBase(suceess_t, PARAMS &&... i_params)
                : m_value(std::forward<PARAMS>(i_params)...), m_has_value(true)
            {
            }

            template <typename... PARAMS>
            constexpr ExpectedBase(unexpected_t, PARAMS &&... i_params)
                : m_error(std::forward<PARAMS>(i_params)...), m_has_value(false)
            {
            }

            ~ExpectedBase()
            {
                if (m_has_value)
                    std::destroy_at(&m_value);
                else
                    std::destroy_at(&m_error);
            }

            union {
                std::conditional_t<std::is_void_v<VALUE>, bool, VALUE> m_value;
                ERROR                                                  m_error;
            };
            bool m_has_value;
        };

        template <typename VALUE, typename ERROR> struct ExpectedBase<VALUE, ERROR, true>
        {
            template <
              typename FIRST_PARAM,
              typename... PARAMS,
              std::enable_if_t<std::is_constructible_v<VALUE, FIRST_PARAM, PARAMS...>> * = nullptr>
            constexpr ExpectedBase(FIRST_PARAM && i_first_param, PARAMS &&... i_params)
                : m_value(
                    std::forward<FIRST_PARAM>(i_first_param), std::forward<PARAMS>(i_params)...),
                  m_has_value(true)
            {
                static_assert(
                  !std::is_constructible_v<ERROR, FIRST_PARAM, PARAMS...>,
                  "ambiguous construction: ERROR is constructible with the "
                  "same arguments");
            }

            template <
              typename FIRST_PARAM,
              typename... PARAMS,
              std::enable_if_t<std::is_constructible_v<ERROR, FIRST_PARAM, PARAMS...>> * = nullptr>
            constexpr ExpectedBase(FIRST_PARAM && i_first_param, PARAMS &&... i_params)
                : m_error(
                    std::forward<FIRST_PARAM>(i_first_param), std::forward<PARAMS>(i_params)...),
                  m_has_value(false)
            {
                static_assert(
                  !std::is_constructible_v<VALUE, FIRST_PARAM, PARAMS...>,
                  "ambiguous construction: VALUE is constructible with the "
                  "same arguments");
            }

            template <typename VAL = VALUE, std::enable_if_t<std::is_void_v<VAL>> * = nullptr>
            constexpr ExpectedBase() : m_value{}, m_has_value(true)
            {
            }

            template <typename... PARAMS>
            constexpr ExpectedBase(suceess_t, PARAMS &&... i_params)
                : m_value(std::forward<PARAMS>(i_params)...), m_has_value(true)
            {
            }

            template <typename... PARAMS>
            constexpr ExpectedBase(unexpected_t, PARAMS &&... i_params)
                : m_error(std::forward<PARAMS>(i_params)...), m_has_value(false)
            {
            }

            union {
                std::conditional_t<std::is_void_v<VALUE>, bool, VALUE> m_value;
                ERROR                                                  m_error;
            };
            bool m_has_value;
        };

    } // namespace detail

    template <typename VALUE, typename ERROR>
    class[[nodiscard]] expected
        : private detail::ExpectedBase<
            VALUE,
            ERROR,
            (std::is_trivially_destructible_v<VALUE> ||
             std::is_void_v<VALUE>)&&std::is_trivially_destructible_v<ERROR>>
    {
        using Base = typename detail::ExpectedBase<
          VALUE,
          ERROR,
          (std::is_trivially_destructible_v<VALUE> ||
           std::is_void_v<VALUE>)&&std::is_trivially_destructible_v<ERROR>>;

      public:
        using detail::ExpectedBase<
          VALUE,
          ERROR,
          (std::is_trivially_destructible_v<VALUE> ||
           std::is_void_v<VALUE>)&&std::is_trivially_destructible_v<ERROR>>::ExpectedBase;

        constexpr expected(const expected &) = default;
        constexpr expected(expected &&)      = default;

        constexpr expected & operator=(const expected &) = default;
        constexpr expected & operator=(expected &&) = default;

        constexpr bool has_value() const noexcept { return Base::m_has_value; }

        constexpr bool has_error() const noexcept { return !Base::m_has_value; }

        /*template <typename VAL = VALUE, std::enable_if_t<!std::is_void_v<VAL>> * = nullptr>
        constexpr operator VAL() const noexcept
        {
            if (Base::m_has_value)
                return Base::m_value;
            else
                throw_error();
        }*/

        template <typename VAL = VALUE, std::enable_if_t<!std::is_void_v<VAL>> * = nullptr>
        constexpr const VAL & value() const &
        {
            if (Base::m_has_value)
                return Base::m_value;
            else
                throw_error();
        }

        template <typename VAL = VALUE, std::enable_if_t<!std::is_void_v<VAL>> * = nullptr>
        constexpr VAL && value() &&
        {
            if (Base::m_has_value)
                return std::move(Base::m_value);
            else
                throw_error();
        }

        constexpr void on_error_except() const
        {
            if (!Base::m_has_value)
                throw_error();
        }

        constexpr const ERROR & error() const noexcept
        {
            EDIACARAN_ASSERT(!Base::m_has_value);
            return Base::m_error;
        }

      private:
        [[noreturn]] constexpr void throw_error() const { throw Base::m_error; }
    };
} // namespace edi
