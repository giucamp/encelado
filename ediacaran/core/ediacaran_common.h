
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2016-2017.


#pragma once
#include <assert.h>
#include <cstdint>
#include <string>
#include <string_view>

/** Assert that on failure should cause an halt of the program. Used only locally in this header. */
#ifdef _MSC_VER
#    define EDIACARAN_CHECKING_ASSERT(bool_expr)                                                                       \
        if (!(bool_expr))                                                                                              \
        {                                                                                                              \
            __debugbreak();                                                                                            \
        }                                                                                                              \
        else                                                                                                           \
            (void)0
#elif defined(__GNUC__)
#    define EDIACARAN_CHECKING_ASSERT(bool_expr)                                                                       \
        if (!(bool_expr))                                                                                              \
        {                                                                                                              \
            __builtin_trap();                                                                                          \
        }                                                                                                              \
        else                                                                                                           \
            (void)0
#else
#    define EDIACARAN_CHECKING_ASSERT(bool_expr) assert(bool_expr)
#endif

/** Macro that tells an invariant to the compiler as hint for the optimizer. . Used only locally in this header. */
#if defined(__clang__)
#    define EDIACARAN_ASSUME(bool_expr)                                                                                \
        _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wassume\"")                              \
          __builtin_assume((bool_expr)) _Pragma("clang diagnostic pop")
#elif defined(_MSC_VER)
#    define EDIACARAN_ASSUME(bool_expr) __assume((bool_expr))
#elif defined(__GNUC__)
#    define EDIACARAN_ASSUME(bool_expr)                                                                                \
        if (!(bool_expr))                                                                                              \
        {                                                                                                              \
            __builtin_unreachable();                                                                                   \
        }                                                                                                              \
        else                                                                                                           \
            (void)0 // https://stackoverflow.com/questions/25667901/assume-clause-in-gcc
#else
#    define EDIACARAN_ASSUME(bool_expr) (void)0
#endif

#ifdef _DEBUG
#    define EDIACARAN_ASSERT EDIACARAN_CHECKING_ASSERT
#    define EDIACARAN_INTERNAL_ASSERT EDIACARAN_CHECKING_ASSERT
#else
#    define EDIACARAN_ASSERT EDIACARAN_ASSUME
#    define EDIACARAN_INTERNAL_ASSERT EDIACARAN_ASSUME
#endif


/** Macro that tells to the compiler that a condition is true in most cases. This is just an hint to the optimizer. */
#if defined(__GNUC__) && !defined(_MSC_VER)
#    define EDIACARAN_LIKELY(bool_expr) (__builtin_expect(bool_expr, true), bool_expr)
#else
#    define EDIACARAN_LIKELY(bool_expr) (bool_expr)
#endif

/** Macro used in some circumstances to avoid inlining of a function, for
        example because the call handles a somewhat rare slow path. */
#ifdef _MSC_VER
#    define EDIACARAN_NO_INLINE __declspec(noinline)
#elif defined(__GNUC__)
#    define EDIACARAN_NO_INLINE __attribute__((noinline))
#else
#    define EDIACARAN_NO_INLINE
#endif

#if __cpp_noexcept_function_type
#    define EDIACARAN_NOEXCEPT_FUNCTION_TYPE noexcept
#else
#    define EDIACARAN_NOEXCEPT_FUNCTION_TYPE
#endif

namespace ediacaran
{
    // workaround for P0426R0 not implemented
    class constexpr_char_traits : public std::char_traits<char>
    {
      public:
        static constexpr void assign(char_type & o_dest, const char_type & i_source) noexcept { o_dest = i_source; }

        static constexpr void assign(char_type * i_dest, size_t i_size, const char_type & i_source) noexcept
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

        constexpr static int compare(const char_type * i_first, const char_type * i_second, size_t i_size) noexcept
        {
            int result = 0;
            for (size_t index = 0; result == 0 && index < i_size; index++)
                result = *i_first - *i_second;
            return result;
        }

        static constexpr bool eq(char_type i_first, char_type i_second) noexcept { return i_first == i_second; }

        static constexpr bool lt(char_type i_first, char_type i_second) noexcept { return i_first < i_second; }
    };

    class string_view : public std::basic_string_view<char, constexpr_char_traits>
    {
      public:
        using basic_string_view<char, constexpr_char_traits>::basic_string_view;

        string_view(const std::string & i_source) noexcept : string_view(i_source.c_str(), i_source.size()) {}

        operator std::string() { return std::string(data(), size()); }
    };

    using hash_t = size_t;

    constexpr hash_t string_hash(const string_view i_source) noexcept
    {
        /*	djb2 - http://www.cse.yorku.ca/~oz/hash.html Bernstein hash function */
        hash_t hash = 5381;
        for (auto c : i_source)
            hash = hash * 33 + c;
        return hash;
    }

    template <typename TYPE, size_t SIZE> constexpr size_t array_size(TYPE (&i_array)[SIZE]) { return SIZE; }

    template <typename CLASS, typename MEMBER_TYPE>
        inline size_t data_member_offset(MEMBER_TYPE (CLASS::*i_member))
    {
        return reinterpret_cast<size_t>(&(reinterpret_cast<CLASS *>(0)->*i_member));
    }

    template <typename CLASS, typename SUBOBJECT>
    const size_t subobject_offset = static_cast<size_t>(
      reinterpret_cast<unsigned char *>(static_cast<SUBOBJECT *>((reinterpret_cast<CLASS *>(sizeof(CLASS))))) -
      reinterpret_cast<unsigned char *>((reinterpret_cast<CLASS *>(sizeof(CLASS)))));
}
