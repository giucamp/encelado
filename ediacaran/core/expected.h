#pragma once
#include <ediacaran/core/ediacaran_common.h>
#include <type_traits>

namespace ediacaran
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
        virtual char const * what() const { return "value not present"; }

      private:
        ERROR m_error;
    };

    namespace detail
    {
        template <typename VALUE, typename ERROR, bool TRIVIAL_DESTRUCTOR> class ExpectedBase;

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
                  "ambiguous construction: ERROR is constructible with the same arguments");
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
                  "ambiguous construction: VALUE is constructible with the same arguments");
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
                  "ambiguous construction: ERROR is constructible with the same arguments");
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
                  "ambiguous construction: VALUE is constructible with the same arguments");
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
        using Base::ExpectedBase;

        constexpr bool has_value() const noexcept { return m_has_value; }

        constexpr bool has_error() const noexcept { return !m_has_value; }

        constexpr operator bool() const noexcept { return m_has_value; }

        template <typename VAL = VALUE, std::enable_if_t<!std::is_void_v<VAL>> * = nullptr>
        constexpr const VAL & value() const
        {
            if (m_has_value)
                return m_value;
            else
                throw_error();
        }

        constexpr void on_error_except() const
        {
            if (!m_has_value)
                throw_error();
        }

        constexpr const ERROR & error() const noexcept
        {
            EDIACARAN_ASSERT(!m_has_value);
            return m_error;
        }

      private:
        [[noreturn]] constexpr void throw_error() const { throw m_error; }
    };
}
