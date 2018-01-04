
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2016-2017.

#pragma once
#include "ediacaran/core/ediacaran_common.h"
#include "ediacaran/reflection/type.h"
#include <array>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace ediacaran
{
    class qualified_type_ptr;
    class char_writer;
    class char_reader;

    /** Retrieves (by value) a qualified_type_ptr associated to the template argument.
        The result is never empty (is_empty() always return false). The template
        argument may be void or any void pointer (with any cv-qualification). */
    template <typename TYPE> constexpr qualified_type_ptr get_qualified_type();

    /** Scoped enum that stores a combination of cv-qualifiers. CV_Flags can be combined and subtracted with the overloaded bitwise operators | and &. */
    enum class CV_Flags
    {
        None     = 0,      /**< No flags */
        Const    = 1 << 0, /**< Set for const types */
        Volatile = 1 << 1, /**< Set for volatile types */
    };

    constexpr inline CV_Flags operator|(CV_Flags i_first, CV_Flags i_seconds) noexcept
    {
        using underlying_type = std::underlying_type<CV_Flags>::type;
        return static_cast<CV_Flags>(static_cast<underlying_type>(i_first) | static_cast<underlying_type>(i_seconds));
    }

    constexpr inline CV_Flags operator&(CV_Flags i_first, CV_Flags i_seconds) noexcept
    {
        using underlying_type = std::underlying_type<CV_Flags>::type;
        return static_cast<CV_Flags>(static_cast<underlying_type>(i_first) & static_cast<underlying_type>(i_seconds));
    }


    /** Lightweight value-class holding a pointer to a type, a number of indirection levels, and the cv-qualification
        (is it \c const? is it \c volatile?) for each indirection level. A qualified_type_ptr can tell:
            - The **number of indirection levels**, that is is the number of '*' or '&' or '&&' appearing in the C++ declaration of
               the type. A non-pointer types has zero indirection levels, while a pointer to a pointer has 2 indirection levels.
               References are considered like const pointer (that is \c get_qualified_type<float&>() == get_qualified_type<float*const>() ).
            - The **primary type**, that is is the type of the first indirection level. For non-pointer types it is the same of the
               final type. For pointer types is always equal to the result of \c get_type<Pointer>(). If an object of has to be
               constructed, copied, or assigned, the primary type is what matters.
            - The **final type**, that is the type of the last indirection level. The final type is the type remaining after stripping away
              all the cv-qualification, pointer and reference parts from the C++ declaration. The final type can be thought as the type of
              the final object, that is the object found indirecting all the indirection levels.
            - **cv-qualification** for every indirection level, that is for every i >= 0 and <= indirection_levels.

        |type                       |primary type    |final type        |# of indirection levels    |const levels        |volatile levels     |
        |---------------------------|:-------------:|:-------------:|:-------------------------:|:-----------------:|:------------------:|
        |float                      |float            |float            |0                            |                    |                     |
        |volatile float             |float            |float            |0                            |                    |0                     |
        |const float &              |void *           |float            |1                            |0, 1                |                     |
        |const void *               |void *           |void             |1                            |1                   |                     |
        |void* const                |void *           |void             |1                            |0                   |                     |
        |float*const*volatile**&    |void *           |float            |5                            |0, 4                |3                     |

        qualified_type_ptr is copyable, assignable and movable.
        Use \c get_qualified_type<TYPE>() to get a \c qualified_type_ptr from a compile-time type.
        Note: <tt> <const int *> </tt> and <tt> <int const *> </tt> are the same C++ type. <br>

        Implementation note: currently qualified_type_ptr has the same size of 2 pointers. Anyway, the user should not rely on this assumption. */
    class qualified_type_ptr final
    {
      public:
        // constants

        /** Maximum indirection levels that this class can handle. This is 14 if \c uintptr_t is 32-bit wide or smaller, 28 otherwise.
            The global function \c get_qualified_type<TYPE>() checks this imit at compile-time (with a \c static_assert). */
        static constexpr size_t s_max_indirection_levels = std::numeric_limits<uintptr_t>::digits <= 32 ? 14 : 28;

        // uintptr_t must be binary
        static_assert(std::numeric_limits<uintptr_t>::radix == 2, "uintptr_t is expected to be binary");


        // getters

        /** Returns the number of indirection levels of the type, that is is the number of '*' or '&' or '&&' appearing in the C++
            declaration. A non-pointer type has zero indirection levels, while a pointer to a pointer has 2 indirection levels.*/
        constexpr size_t indirection_levels() const noexcept { return m_indirection_levels; }

        /** Retrieves the primary type, that is the type at the 0-th indirection level.
            If the type is empty (= default constructed) the primary type is nullptr. Otherwise is != \c nullptr. */
        constexpr const type * primary_type() const noexcept;

        /** Retrieves the final type, that is the type at the last indirection level.
            If the type is empty (= default constructed) the final type is nullptr. Otherwise is != \c nullptr. */
        constexpr const type * final_type() const noexcept { return m_final_type; }

        /** Retrieves whether a given indirection level has a const qualifier.
            @param i_indirection_level indirection level (must be <= \c indirection_levels()) for which the consteness is
                queried. In the type: <tt>float const**</tt>, <tt>is_const(0)</tt> and <tt>is_const(1)</tt> return \c false,
                while <tt>is_const(2)</tt> returns \c true. */
        constexpr bool is_const(size_t i_indirection_level) const noexcept;

        /** Retrieves whether a given indirection level has a volatile qualifier.
            @param i_indirection_level indirection level (must be <= \c indirection_levels()) for which the volatileness is
                queried. In the type: <tt>float volatile**</tt>, <tt>is_volatile(0)</tt> and <tt>is_volatile(1)</tt> return
                \c false, while \c <tt>is_volatile(2)</tt> returns \c true. */
        constexpr bool is_volatile(size_t i_indirection_level) const noexcept;

        /** Returns whether is empty (that is default constructed). */
        constexpr bool is_empty() const noexcept { return final_type() == nullptr; }

        /** Retrieves whether the type rapresent a pointer, that is indirection_levels() > 0 */
        constexpr bool is_pointer() const noexcept { return indirection_levels() > 0; }


        // derived getters

        /** Retrieves a \c CV_Flags that specifies the cv-qualification for the specified indirection level.
            Given the type <tt>get_qualified_type<float volatile*const volatile*const*>()</tt>:
                - \c cv_flags(0) returns <tt> CV_Flags::None </tt>
                - \c cv_flags(1) returns <tt> CV_Flags::Const </tt>
                - \c cv_flags(2) returns <tt> CV_Flags::Const | CV_Flags::Volatile </tt>
                - \c cv_flags(3) returns <tt> CV_Flags::Volatile </tt>

            Implementation note: \c cv_flags() is impemented using \c is_const() and \c is_volatile().
            @param i_indirection_level indirection level for which the qualification is queried. It must be <= \c indirection_levels() */
        constexpr CV_Flags cv_flags(size_t i_indirection_level) const noexcept
        {
            return (is_const(i_indirection_level) ? CV_Flags::Const : CV_Flags::None) |
                   (is_volatile(i_indirection_level) ? CV_Flags::Volatile : CV_Flags::None);
        }

        constexpr size_t constness_word() const noexcept { return m_constness_word; }

        constexpr size_t volatileness_word() const noexcept { return m_volatileness_word; }


        // special functions

        constexpr qualified_type_ptr(
          const type * i_final_type, size_t i_indirection_levels, size_t i_constness_word,
          size_t i_volatileness_word) noexcept;

        /** Constructs an empty qualified_type_ptr (is_empty() will return true). The object may be later the destination of an assignment, changing its state. */
        constexpr qualified_type_ptr() noexcept;

        /** Constructs a non-empty qualified_type_ptr from a final type and an array of CV_Flags's that specifies the cv-qualifiers of the indirection levels.
            The size of the array of CV_Flags's determines the number of indirection levels.
            In the the following code <tt>q_type_ptr_1 == q_type_ptr_2</tt>:<br>
            <tt>qualified_type_ptr q_type_ptr_1(get_type<void>(), { CV_Flags::Const | CV_Flags::Volatile, CV_Flags::None, CV_Flags::Volatile });<br>
            qualified_type_ptr q_type_ptr_2 = get_qualified_type<void volatile * * volatile const >();</tt><br>
            @param i_final_type final type. May be get_type<void>().
            @param i_cv_flags cv-qualification for each indirection level. The n-th element of this array specifies a combination of cv flags for the n-th indirection
                level. The number of indirection levels of the type is the size of this array, minus 1. So, to construct a pointer to a pointer, specify an array
                of 3 elements. If the array is empty, the number of indirection levels is zero. */
        constexpr qualified_type_ptr(const type & i_final_type, const CV_Flags * i_cv_flags, size_t i_cv_flags_size);

        constexpr qualified_type_ptr(const type & i_final_type, const std::initializer_list<CV_Flags> & i_cv_flags)
            : qualified_type_ptr(i_final_type, i_cv_flags.begin(), i_cv_flags.end() - i_cv_flags.begin())
        {
        }

        /** Copies from the source qualified_type_ptr */
        constexpr qualified_type_ptr(const qualified_type_ptr & i_source) noexcept = default;

        /** Assigns from the source qualified_type_ptr */
        constexpr qualified_type_ptr & operator=(const qualified_type_ptr & i_source) noexcept = default;


        // comparison

        /** Returns true whether two QualifiedTypePtrs are indistinguishable */
        constexpr bool operator==(const qualified_type_ptr & i_source) const;

        /** Returns false whether two QualifiedTypePtrs are indistinguishable */
        constexpr bool operator!=(const qualified_type_ptr & i_source) const { return !operator==(i_source); }

      private:
        template <typename TYPE> friend constexpr qualified_type_ptr get_qualified_type();

        friend char_reader & operator>>(char_reader & i_source, qualified_type_ptr & o_dest_qualified_type);

      private: // data members (currently a qualified_type_ptr is big as two pointers)
        const type * m_final_type;
        uintptr_t    m_indirection_levels : (std::numeric_limits<uintptr_t>::digits - s_max_indirection_levels * 2);
        uintptr_t    m_constness_word : s_max_indirection_levels;
        uintptr_t    m_volatileness_word : s_max_indirection_levels;
    };

    char_writer & operator<<(char_writer & o_dest, const qualified_type_ptr & i_source) noexcept;

    bool try_parse(qualified_type_ptr & o_dest, char_reader & i_source, char_writer & o_error_dest) noexcept;

} // namespace ediacaran

#include "qualified_type_ptr.inl"
