
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "ediacaran/core/array_view.h"
#include "ediacaran/core/comma_separated_names.h"
#include "ediacaran/reflection/parameter.h"
#include "ediacaran/reflection/qualified_type_ptr.h"
#include "ediacaran/reflection/type.h"
#include <ediacaran/core/array.h>
#include <utility>

namespace edi
{
    class function
    {
      public:
        using invoke_function = void (*)(
          void * i_dest_object, void * o_return_value_dest, const void * const * i_parameters);

        constexpr function(
          const char *                        i_name,
          cv_qualification                    i_cv_qualification,
          qualified_type_ptr                  i_return_qualified_type,
          array_view<const parameter> const & i_parameters,
          const char *                        i_parameter_names,
          invoke_function                     i_invoke_function)
            : m_name(i_name), m_invoke_function(i_invoke_function),
              m_qualified_return_type(
                i_return_qualified_type.set_qualification(0, i_cv_qualification)),
              m_parameters(i_parameters), m_parameter_names(i_parameter_names)
        {
            size_t parameter_names_count = 0;
            for (auto it = m_parameter_names.cbegin(); it != m_parameter_names.end(); it++)
                parameter_names_count++;
            if (m_parameters.size() != parameter_names_count)
            {
                except<std::runtime_error>(
                  "The function ",
                  i_name,
                  " has ",
                  m_parameters.size(),
                  " parameters, ",
                  parameter_names_count,
                  " parameter names provided");
            }
        }

        constexpr string_view name() const noexcept
        {
            // workaround for gcc considering the comparison of two char* non-constexpr
            return string_view(m_name, string_view::traits_type::length(m_name));
        }

        constexpr qualified_type_ptr qualified_return_type() const noexcept
        {
            return qualified_type_ptr(m_qualified_return_type)
              .set_qualification(0, cv_qualification::no_q);
        }

        constexpr array_view<const parameter> const & parameters() const noexcept
        {
            return m_parameters;
        }

        void invoke(
          void * i_dest_object, void * o_return_value_dest, const void * const * i_parameters) const
        {
            (*m_invoke_function)(i_dest_object, o_return_value_dest, i_parameters);
        }

        constexpr comma_separated_names const & parameter_names() const noexcept
        {
            return m_parameter_names;
        }

        constexpr cv_qualification qualification() const noexcept
        {
            return m_qualified_return_type.qualification(0);
        }

      private:
        const char * const    m_name;
        invoke_function const m_invoke_function;
        qualified_type_ptr const
                                          m_qualified_return_type; /**< The cv-qualification of the first indirection level is the
                                                                        cv-qualification of the function */
        array_view<const parameter> const m_parameters;
        comma_separated_names const       m_parameter_names;
    };

    namespace detail
    {
        template <typename METHOD_TYPE> struct MethodTraits;

        template <typename METHOD_TYPE, METHOD_TYPE METHOD, typename INDEX_SEQUENCE>
        struct FunctionInvoker;

#ifdef __clang__
        /* silent warning: suggest braces around initialization of subobject,
        because double braces fail to compile with an array of zero size */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif

#define EDIACARAN_FUNC_REFL_CV_QUALFICATION
#define EDIACARAN_FUNC_REFL_CV_QUALFICATION_V cv_qualification::no_q
#define EDIACARAN_FUNC_REFL_NOEXCEPT_SPEC
#include <ediacaran/reflection/detail/function_accessor.h>
#define EDIACARAN_FUNC_REFL_CV_QUALFICATION
#define EDIACARAN_FUNC_REFL_CV_QUALFICATION_V cv_qualification::no_q
#define EDIACARAN_FUNC_REFL_NOEXCEPT_SPEC noexcept
#include <ediacaran/reflection/detail/function_accessor.h>

#define EDIACARAN_FUNC_REFL_CV_QUALFICATION const
#define EDIACARAN_FUNC_REFL_CV_QUALFICATION_V cv_qualification::const_q
#define EDIACARAN_FUNC_REFL_NOEXCEPT_SPEC
#include <ediacaran/reflection/detail/function_accessor.h>
#define EDIACARAN_FUNC_REFL_CV_QUALFICATION const
#define EDIACARAN_FUNC_REFL_CV_QUALFICATION_V cv_qualification::const_q
#define EDIACARAN_FUNC_REFL_NOEXCEPT_SPEC noexcept
#include <ediacaran/reflection/detail/function_accessor.h>

#define EDIACARAN_FUNC_REFL_CV_QUALFICATION volatile
#define EDIACARAN_FUNC_REFL_CV_QUALFICATION_V cv_qualification::volatile_q
#define EDIACARAN_FUNC_REFL_NOEXCEPT_SPEC
#include <ediacaran/reflection/detail/function_accessor.h>
#define EDIACARAN_FUNC_REFL_CV_QUALFICATION volatile
#define EDIACARAN_FUNC_REFL_CV_QUALFICATION_V cv_qualification::volatile_q
#define EDIACARAN_FUNC_REFL_NOEXCEPT_SPEC noexcept
#include <ediacaran/reflection/detail/function_accessor.h>

#define EDIACARAN_FUNC_REFL_CV_QUALFICATION const volatile
#define EDIACARAN_FUNC_REFL_NOEXCEPT_SPEC
#define EDIACARAN_FUNC_REFL_CV_QUALFICATION_V                                                      \
    cv_qualification::const_q | cv_qualification::volatile_q
#include <ediacaran/reflection/detail/function_accessor.h>
#define EDIACARAN_FUNC_REFL_CV_QUALFICATION const volatile
#define EDIACARAN_FUNC_REFL_CV_QUALFICATION_V                                                      \
    cv_qualification::const_q | cv_qualification::volatile_q
#define EDIACARAN_FUNC_REFL_NOEXCEPT_SPEC noexcept
#include <ediacaran/reflection/detail/function_accessor.h>

#ifdef __clang__
#pragma clang diagnostic pop
#endif
    } // namespace detail

    template <typename METHOD_TYPE, METHOD_TYPE METHOD>
    constexpr function make_function(const char * i_name, const char * i_parameter_names = "")
    {
        using function_invoker = detail::FunctionInvoker<
          METHOD_TYPE,
          METHOD,
          std::make_index_sequence<detail::MethodTraits<METHOD_TYPE>::parameter_count>>;
        return function(
          i_name,
          detail::MethodTraits<METHOD_TYPE>::qualification,
          get_qualified_type<typename function_invoker::return_type>(),
          function_invoker::parameters,
          i_parameter_names,
          &function_invoker::func);
    }

} // namespace edi
