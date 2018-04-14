
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "ediacaran/core/ediacaran_common.h"
#include "ediacaran/reflection/cv_qualification.h"
#include "ediacaran/reflection/type.h"
#include <ediacaran/core/array.h>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace edi
{
    class qualified_type_ptr;
    class char_writer;
    class char_reader;

    /** Retrieves (by value) a qualified_type_ptr associated to the template argument.
        The result is never empty (is_empty() always return false). The template
        argument may be void or any void pointer (with any cv-qualification). */
    template <typename TYPE> constexpr qualified_type_ptr get_qualified_type();

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
        static constexpr size_t s_max_indirection_levels =
          std::numeric_limits<uintptr_t>::digits <= 32 ? 14 : 28;

        // uintptr_t must be binary
        static_assert(
          std::numeric_limits<uintptr_t>::radix == 2, "uintptr_t is expected to be binary");


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

        constexpr qualified_type_ptr &
          set_qualification(size_t i_indirection_level, cv_qualification i_cv) noexcept
        {
            auto const mask             = uintptr_t(1) << i_indirection_level;
            auto const constness_add    = edi::is_const(i_cv) ? mask : uintptr_t(0);
            m_constness_word            = (m_constness_word & ~mask) | constness_add;
            auto const volatileness_add = edi::is_volatile(i_cv) ? mask : uintptr_t(0);
            m_volatileness_word         = (m_volatileness_word & ~mask) | volatileness_add;
            return *this;
        }

        // derived getters

        /** Retrieves a \c cv_qualification that specifies the cv-qualification for the specified indirection level.
            Given the type <tt>get_qualified_type<float volatile*const volatile*const*>()</tt>:
                - \c qualification(0) returns <tt> cv_qualification::no_q </tt>
                - \c qualification(1) returns <tt> cv_qualification::const_q </tt>
                - \c qualification(2) returns <tt> cv_qualification::const_q | cv_qualification::volatile_q </tt>
                - \c qualification(3) returns <tt> cv_qualification::volatile_q </tt>

            Implementation note: \c qualification() is impemented using \c is_const() and \c is_volatile().
            @param i_indirection_level indirection level for which the qualification is queried. It must be <= \c indirection_levels() */
        constexpr cv_qualification qualification(size_t i_indirection_level) const noexcept
        {
            return (is_const(i_indirection_level) ? cv_qualification::const_q
                                                  : cv_qualification::no_q) |
                   (is_volatile(i_indirection_level) ? cv_qualification::volatile_q
                                                     : cv_qualification::no_q);
        }

        constexpr size_t constness_word() const noexcept { return m_constness_word; }

        constexpr size_t volatileness_word() const noexcept { return m_volatileness_word; }

        // special functions

        constexpr qualified_type_ptr(
          const type * i_final_type,
          size_t       i_indirection_levels,
          size_t       i_constness_word,
          size_t       i_volatileness_word) noexcept;

        /** Constructs an empty qualified_type_ptr (is_empty() will return true). The object may be later the destination of an assignment, changing its state. */
        constexpr qualified_type_ptr() noexcept;

        /** Constructs a non-empty qualified_type_ptr from a final type and an array of cv_qualification's that specifies the cv-qualifiers of the indirection levels.
            The size of the array of cv_qualification's determines the number of indirection levels.
            In the the following code <tt>q_type_ptr_1 == q_type_ptr_2</tt>:<br>
            <tt>qualified_type_ptr q_type_ptr_1(get_type<void>(), { cv_qualification::const_q | cv_qualification::volatile_q, cv_qualification::no_q, cv_qualification::volatile_q });<br>
            qualified_type_ptr q_type_ptr_2 = get_qualified_type<void volatile * * volatile const >();</tt><br>
            @param i_final_type final type. May be get_type<void>().
            @param i_cv_flags cv-qualification for each indirection level. The n-th element of this array specifies a combination of cv flags for the n-th indirection
                level. The number of indirection levels of the type is the size of this array, minus 1. So, to construct a pointer to a pointer, specify an array
                of 3 elements. If the array is empty, the number of indirection levels is zero. */
        constexpr qualified_type_ptr(
          const type & i_final_type, const cv_qualification * i_cv_flags, size_t i_cv_flags_size);

        constexpr qualified_type_ptr(
          const type & i_final_type, const std::initializer_list<cv_qualification> & i_cv_flags)
            : qualified_type_ptr(
                i_final_type, i_cv_flags.begin(), i_cv_flags.end() - i_cv_flags.begin())
        {
        }

        /** Copies from the source qualified_type_ptr */
        constexpr qualified_type_ptr(const qualified_type_ptr & i_source) noexcept = default;

        /** Assigns from the source qualified_type_ptr */
        constexpr qualified_type_ptr &
          operator=(const qualified_type_ptr & i_source) noexcept = default;


        // comparison

        /** Returns true whether two QualifiedTypePtrs are indistinguishable */
        constexpr friend bool operator==(
          const qualified_type_ptr & i_first, const qualified_type_ptr & i_second) noexcept
        {
            if (i_first.m_final_type == nullptr)
            {
                return i_second.m_final_type == nullptr;
            }

            return i_second.m_final_type != nullptr &&
                   i_first.m_final_type->name() == i_second.m_final_type->name() &&
                   i_first.m_indirection_levels == i_second.m_indirection_levels &&
                   i_first.m_constness_word == i_second.m_constness_word &&
                   i_first.m_volatileness_word == i_second.m_volatileness_word;
        }

        /** Returns false whether two QualifiedTypePtrs are indistinguishable */
        constexpr friend bool operator!=(
          const qualified_type_ptr & i_first, const qualified_type_ptr & i_second) noexcept
        {
            return !(i_first == i_second);
        }

      private:
        template <typename TYPE> friend constexpr qualified_type_ptr get_qualified_type();

        friend char_reader &
          operator>>(char_reader & i_source, qualified_type_ptr & o_dest_qualified_type);

        friend constexpr char_writer &
          operator<<(char_writer & o_dest, const qualified_type_ptr & i_source) noexcept;

      private: // data members (currently a qualified_type_ptr is big as two pointers)
        const type * m_final_type;
        uintptr_t    m_indirection_levels
            : (std::numeric_limits<uintptr_t>::digits - s_max_indirection_levels * 2);
        uintptr_t m_constness_word : s_max_indirection_levels;
        uintptr_t m_volatileness_word : s_max_indirection_levels;
    };

    constexpr char_writer &
      operator<<(char_writer & o_dest, const qualified_type_ptr & i_source) noexcept
    {
        if (i_source.m_final_type != nullptr)
        {
            o_dest << i_source.m_final_type->name();

            uintptr_t level = i_source.indirection_levels();
            for (;;)
            {
                if (i_source.is_const(level))
                {
                    o_dest << " const";
                }

                if (i_source.is_volatile(level))
                {
                    o_dest << " volatile";
                }

                if (level == 0)
                    break;

                level--;
                o_dest << " *";
            }
        }
        return o_dest;
    }

    expected<void, parse_error> parse(qualified_type_ptr & o_dest, char_reader & i_source) noexcept;

} // namespace edi

#include "qualified_type_ptr.inl"
