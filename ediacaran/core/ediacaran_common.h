
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <assert.h>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>

/** Assert that on failure should cause an halt of the program. Used only locally in this header. */
#ifdef _MSC_VER
#define EDIACARAN_CHECKING_ASSERT(bool_expr)                                                       \
    if (!(bool_expr))                                                                              \
    {                                                                                              \
        __debugbreak();                                                                            \
    }                                                                                              \
    else                                                                                           \
        (void)0
#elif defined(__GNUC__)
#define EDIACARAN_CHECKING_ASSERT(bool_expr)                                                       \
    if (!(bool_expr))                                                                              \
    {                                                                                              \
        __builtin_trap();                                                                          \
    }                                                                                              \
    else                                                                                           \
        (void)0
#else
#define EDIACARAN_CHECKING_ASSERT(bool_expr) assert(bool_expr)
#endif

/** Macro that tells an invariant to the compiler as hint for the optimizer. Used only locally in this header. */
#if defined(__clang__)
#define EDIACARAN_ASSUME(bool_expr)                                                                \
    _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wassume\"")              \
      __builtin_assume((bool_expr)) _Pragma("clang diagnostic pop")
#elif defined(_MSC_VER)
#define EDIACARAN_ASSUME(bool_expr) __assume((bool_expr))
#elif defined(__GNUC__)
#define EDIACARAN_ASSUME(bool_expr)                                                                \
    if (!(bool_expr))                                                                              \
    {                                                                                              \
        __builtin_unreachable();                                                                   \
    }                                                                                              \
    else                                                                                           \
        (void)0 // https://stackoverflow.com/questions/25667901/assume-clause-in-gcc
#else
#define EDIACARAN_ASSUME(bool_expr) (void)0
#endif

#ifdef _DEBUG
#define EDIACARAN_ASSERT EDIACARAN_CHECKING_ASSERT
#define EDIACARAN_INTERNAL_ASSERT EDIACARAN_CHECKING_ASSERT
#else
#define EDIACARAN_ASSERT EDIACARAN_ASSUME
#define EDIACARAN_INTERNAL_ASSERT EDIACARAN_ASSUME
#endif


/** Macro that tells to the compiler that a condition is true in most cases. This is just an hint to the optimizer. */
#if defined(__GNUC__) && !defined(_MSC_VER)
#define EDIACARAN_LIKELY(bool_expr) (__builtin_expect(bool_expr, true), bool_expr)
#else
#define EDIACARAN_LIKELY(bool_expr) (bool_expr)
#endif

/** Macro used in some circumstances to avoid inlining of a function, for
        example because the call handles a somewhat rare slow path. */
#ifdef _MSC_VER
#define EDIACARAN_NO_INLINE __declspec(noinline)
#elif defined(__GNUC__)
#define EDIACARAN_NO_INLINE __attribute__((noinline))
#else
#define EDIACARAN_NO_INLINE
#endif

namespace edi
{
    class unsupported_error : public std::runtime_error
    {
      public:
        using std::runtime_error::runtime_error;
    };

    class constness_violation : public std::runtime_error
    {
      public:
        using std::runtime_error::runtime_error;
    };

    class null_pointer_indirection : public std::runtime_error
    {
      public:
        using std::runtime_error::runtime_error;
    };

    class mismatching_arguments : public std::runtime_error
    {
      public:
        using std::runtime_error::runtime_error;
    };

    template <typename TYPE> using is_trivially_serializable = std::bool_constant<std::is_fundamental_v<TYPE> || std::is_enum_v<TYPE>>;

    template <typename TYPE>
    constexpr bool is_trivially_serializable_v = is_trivially_serializable<TYPE>::value;

    template <typename EXCEPTION_TYPE>[[noreturn]] constexpr void except(const char * i_message)
    {
        throw EXCEPTION_TYPE(i_message);
    }

    struct end_marker_t
    {
    };
    constexpr end_marker_t end_marker;

    // workaround for P0426R0 not implemented
    class constexpr_char_traits : public std::char_traits<char>
    {
      public:
        static constexpr void assign(char_type & o_dest, const char_type & i_source) noexcept
        {
            o_dest = i_source;
        }

        static constexpr void
          assign(char_type * i_dest, size_t i_size, const char_type & i_source) noexcept
        {
            for (size_t index = 0; index < i_size; index++)
                i_dest[index] = i_source;
        }

        constexpr static size_t length(const char_type * i_string) noexcept
        {
            auto curr = i_string;
            while (*curr)
                curr++;
            return curr - i_string;
        }

        constexpr static int
          compare(const char_type * i_first, const char_type * i_second, size_t i_size) noexcept
        {
            int result = 0;
            for (size_t index = 0; result == 0 && index < i_size; index++, i_first++, i_second++)
                result = *i_first - *i_second;
            return result;
        }

        static constexpr bool eq(char_type i_first, char_type i_second) noexcept
        {
            return i_first == i_second;
        }

        static constexpr bool lt(char_type i_first, char_type i_second) noexcept
        {
            return i_first < i_second;
        }
    };

    class string_view : public std::basic_string_view<char, constexpr_char_traits>
    {
      public:
        using basic_string_view<char, constexpr_char_traits>::basic_string_view;

        constexpr string_view() noexcept {}

        string_view(const std::string & i_source) noexcept
            : string_view(i_source.c_str(), i_source.size())
        {
        }

        operator std::string() const { return std::string(data(), size()); }
    };

    inline std::string operator+(const std::string & i_first, const string_view & i_second)
    {
        std::string res = i_first;
        res.append(i_second.data(), i_second.size());
        return res;
    }

    inline std::string & operator+=(std::string & i_first, const string_view & i_second)
    {
        i_first.append(i_second.data(), i_second.size());
        return i_first;
    }

    template <typename CONTAINER>
    constexpr auto find_named(CONTAINER && i_container, const string_view & i_name)
    {
        auto const end = std::forward<CONTAINER>(i_container).end();
        for (auto it = std::forward<CONTAINER>(i_container).begin(); it != end; it++)
        {
            if (it->name() == i_name)
                return it;
        }
        return end;
    }

    namespace detail
    {
        template <typename TYPE> inline void * ptr_remove_cv(TYPE * i_ptr)
        {
            return const_cast<std::remove_cv_t<TYPE> *>(i_ptr);
        }
    } // namespace detail
} // namespace edi
