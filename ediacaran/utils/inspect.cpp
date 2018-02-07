
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <ediacaran/core/ediacaran_common.h>
#include <ediacaran/core/string_builder.h>
#include <ediacaran/reflection/reflection.h>
#include <ediacaran/utils/inspect.h>
#include <vector>

namespace edi
{
    property_inspector inspect_properties(const raw_ptr & i_target)
    {
        return property_inspector(i_target);
    }

    property_inspector::iterator::iterator(const raw_ptr & i_target) noexcept
        : m_target(i_target.full_indirection())
    {
        auto const final_type = m_target.qualified_type().final_type();
        if (final_type->is_class())
        {
            m_class     = static_cast<const class_type *>(final_type);
            m_subobject = const_cast<void *>(m_target.object());
            m_property  = m_class->properties().data();
            if (m_class->properties().empty())
            {
                next_base();
            }
        }
    }

    void property_inspector::iterator::next_base() noexcept
    {
        if (m_class == m_target.qualified_type().final_type())
        {
            EDIACARAN_INTERNAL_ASSERT(m_base_index == 0);
        }
        else
        {
            m_base_index++;
        }

        for (;;)
        {
            auto const complete_class =
              static_cast<const class_type *>(m_target.qualified_type().final_type());
            if (m_base_index < complete_class->bases().size())
            {
                auto const & base = complete_class->bases()[m_base_index];
                m_class           = &base.get_class();
                m_subobject       = const_cast<void *>(base.up_cast(m_target.object()));
                if (!m_class->properties().empty())
                {
                    m_property = m_class->properties().data();
                    break;
                }
            }
            else
            {
                m_property = nullptr;
                break;
            }

            m_base_index++;
        }
    }

    raw_ptr property_inspector::iterator::get_prop_value()
    {
        auto const & property_type = m_property->qualified_type();
        auto         value         = const_cast<void *>(m_property->get_inplace(m_subobject));
        if (value == nullptr)
        {
            value = m_dyn_value.manual_construct(property_type, [&](void * i_dest) {
                char        error_msg[512];
                char_writer error_writer(error_msg);
                if (!m_property->get(m_subobject, i_dest, error_writer))
                {
                    except<std::runtime_error>(error_msg);
                }
            });
        }
        return raw_ptr(value, property_type);
    }

    void property_inspector::iterator::set_prop_value(const raw_ptr & i_value)
    {
        if (i_value.empty())
        {
            except<std::runtime_error>("The value is empty");
        }

        if (i_value.qualified_type() != m_property->qualified_type())
        {
            except<std::runtime_error>(
              "The property ",
              m_property->name(),
              " expects a ",
              m_property->qualified_type(),
              ", a ",
              i_value.qualified_type(),
              " was provided");
        }

        char        error[512];
        char_writer err_writer(error);
        if (!m_property->set(m_subobject, i_value.object(), err_writer))
        {
            except<std::runtime_error>(error);
        }
    }

    void property_inspector::iterator::set_prop_value(char_reader & i_source)
    {
        auto const & property_type = m_property->qualified_type();
        if (!property_type.is_pointer())
        {
            m_dyn_value.assign(m_property->qualified_type());
            auto const parse_result =
              property_type.final_type()->parse(const_cast<void *>(m_dyn_value.object()), i_source);
            parse_result.on_error_except();
            char        error[512];
            char_writer error_writer(error);
            if (!m_property->set(m_subobject, m_dyn_value.object(), error_writer))
            {
                except<std::runtime_error>(error);
            }
        }
        else
        {
            auto const final_value  = get_prop_value().full_indirection();
            auto const parse_result = final_value.qualified_type().final_type()->parse(
              const_cast<void *>(m_dyn_value.object()), i_source);
            parse_result.on_error_except();
        }
    }

    void property_inspector::iterator::set_prop_value(const string_view & i_source)
    {
        char_reader reader(i_source);
        set_prop_value(reader);
        if (reader.remaining_chars() != 0)
        {
            throw parse_error::tailing_chars;
        }
    }

    dyn_value get_property_value(const raw_ptr & i_target, char_reader & i_property_name_source)
    {
        if (i_target.empty())
        {
            except<std::logic_error>("Missing property target");
        }

        auto const property_name = try_parse_identifier(i_property_name_source);
        if (property_name.empty())
        {
            except<std::logic_error>("Missing property name");
        }
        for (auto const & property : inspect_properties(i_target))
        {
            if (property.name() == property_name)
            {
                return property.get_value();
            }
        }

        except<std::logic_error>("Property not found: ", property_name);
    }

    dyn_value
      get_property_value(const raw_ptr & i_target, const string_view & i_property_name_source)
    {
        char_reader source(i_property_name_source);
        auto        res = get_property_value(i_target, source);
        if (source.remaining_chars() != 0)
        {
            throw parse_error::tailing_chars;
        }
        return res;
    }

    namespace detail
    {
        template <typename VALUE>
        void set_property_value_impl(
          const raw_ptr & i_target, char_reader & i_property_name_source, VALUE && i_value)
        {
            auto const property_name = try_parse_identifier(i_property_name_source);
            if (property_name.empty())
            {
                except<std::runtime_error>("Missing property name");
            }

            if (i_target.empty())
            {
                except<std::runtime_error>("Missing property target");
            }

            bool found = false;
            for (auto const & prop : inspect_properties(i_target))
            {
                if (prop.name() == property_name)
                {
                    prop.set_value(std::forward<VALUE>(i_value));
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                except<std::runtime_error>("Could not find the property ", property_name);
            }
        }
    } // namespace detail

    void set_property_value(
      const raw_ptr & i_target, char_reader & i_property_name_source, const raw_ptr & i_value)
    {
        if (i_value.empty())
        {
            except<std::runtime_error>("Missing property value");
        }
        detail::set_property_value_impl(i_target, i_property_name_source, i_value);
    }

    void set_property_value(
      const raw_ptr & i_target, const string_view & i_property_name, const raw_ptr & i_value)
    {
        char_reader property_name(i_property_name);
        set_property_value(i_target, property_name, i_value);
        if (property_name.remaining_chars() != 0)
        {
            throw parse_error::tailing_chars;
        }
    }

    void set_property_value(
      const raw_ptr & i_target, char_reader & i_property_name_source, char_reader & i_value_source)
    {
        detail::set_property_value_impl(i_target, i_property_name_source, i_value_source);
    }

    void set_property_value(
      const raw_ptr & i_target, const string_view & i_property_name, char_reader & i_value_source)
    {
        char_reader property_name(i_property_name);
        set_property_value(i_target, property_name, i_value_source);
        if (property_name.remaining_chars() != 0)
        {
            throw parse_error::tailing_chars;
        }
    }

    void set_property_value(
      const raw_ptr &     i_target,
      char_reader &       i_property_name_source,
      const string_view & i_value_source)
    {
        detail::set_property_value_impl(i_target, i_property_name_source, i_value_source);
    }

    void set_property_value(
      const raw_ptr &     i_target,
      const string_view & i_property_name,
      const string_view & i_value_source)
    {
        char_reader property_name(i_property_name);
        set_property_value(i_target, property_name, i_value_source);
        if (property_name.remaining_chars() != 0)
        {
            throw parse_error::tailing_chars;
        }
    }

    function_inspector inspect_functions(const raw_ptr & i_target)
    {
        return function_inspector(i_target);
    }

    function_inspector::iterator::iterator(const raw_ptr & i_target) noexcept
        : m_target(i_target.full_indirection())
    {
        auto const final_type = m_target.qualified_type().final_type();
        if (final_type->is_class())
        {
            m_class     = static_cast<const class_type *>(final_type);
            m_subobject = const_cast<void *>(m_target.object());
            m_function  = m_class->functions().data();
            if (m_class->functions().empty())
            {
                next_base();
            }
        }
    }

    void function_inspector::iterator::next_base() noexcept
    {
        if (m_class == m_target.qualified_type().final_type())
        {
            EDIACARAN_INTERNAL_ASSERT(m_base_index == 0);
        }
        else
        {
            m_base_index++;
        }

        for (;;)
        {
            auto const complete_class =
              static_cast<const class_type *>(m_target.qualified_type().final_type());
            if (m_base_index < complete_class->bases().size())
            {
                auto const & base = complete_class->bases()[m_base_index];
                m_class           = &base.get_class();
                m_subobject       = const_cast<void *>(base.up_cast(m_target.object()));
                if (!m_class->functions().empty())
                {
                    m_function = m_class->functions().data();
                    break;
                }
            }
            else
            {
                m_function = nullptr;
                break;
            }

            m_base_index++;
        }
    }

    raw_ptr function_inspector::iterator::invoke(const array_view<const raw_ptr> & i_arguments)
    {
        auto const & parameters = m_function->parameters();

        if (i_arguments.size() != parameters.size())
        {
            except<mismatching_arguments>(
              "The function ",
              m_function->name(),
              " expects ",
              parameters.size(),
              " arguments, ",
              i_arguments.size(),
              " were provided");
        }

        std::vector<void *> arguments(i_arguments.size());
        for (size_t i = 0; i < i_arguments.size(); i++)
        {
            if (i_arguments[i].qualified_type() != parameters[i].qualified_type())
            {
                auto const parameter_name = *std::next(m_function->parameter_names().begin(), i);
                except<mismatching_arguments>(
                  "The argument ",
                  i,
                  " (",
                  parameter_name,
                  ") of the function ",
                  m_function->name(),
                  " is expected to have type ",
                  parameters[i].qualified_type(),
                  ", a ",
                  i_arguments[i].qualified_type(),
                  " was provided");
            }
            arguments[i] = const_cast<void *>(i_arguments[i].object());
        }

        auto const value =
          m_dyn_value.manual_construct(m_function->qualified_return_type(), [&](void * i_dest) {
              m_function->invoke(m_subobject, i_dest, arguments.data());
          });
        return raw_ptr(value, m_function->qualified_return_type());
    }

    raw_ptr function_inspector::iterator::invoke(const array_view<string_view> & i_arguments)
    {
        auto const & parameters = m_function->parameters();

        if (i_arguments.size() != parameters.size())
        {
            except<mismatching_arguments>(
              "The function ",
              m_function->name(),
              " expects ",
              parameters.size(),
              " arguments, ",
              i_arguments.size(),
              " were provided");
        }

        std::vector<dyn_value> dyn_arguments;
        std::vector<void *>    arguments;
        dyn_arguments.reserve(parameters.size());
        arguments.reserve(parameters.size());
        for (size_t i = 0; i < i_arguments.size(); i++)
        {
            dyn_arguments.push_back(parse_value(parameters[i].qualified_type(), i_arguments[i]));
            arguments.push_back(const_cast<void *>(dyn_arguments.back().object()));
        }

        auto const value =
          m_dyn_value.manual_construct(m_function->qualified_return_type(), [&](void * i_dest) {
              m_function->invoke(m_subobject, i_dest, arguments.data());
          });
        return raw_ptr(value, m_function->qualified_return_type());
    }

    raw_ptr function_inspector::iterator::invoke(char_reader & i_arguments_source)
    {
        auto const & parameters = m_function->parameters();

        std::vector<dyn_value> dyn_arguments;
        std::vector<void *>    arguments;
        dyn_arguments.reserve(parameters.size());
        arguments.reserve(parameters.size());

        for (size_t i = 0; i < parameters.size(); i++)
        {
            (void)accept(spaces, i_arguments_source);

            auto const & parameter = parameters[i];
            auto &       dyn_value = dyn_arguments.emplace_back(
              parse_value(parameter.qualified_type(), i_arguments_source));
            arguments.push_back(const_cast<void *>(dyn_value.object()));

            if (i + 1 < parameters.size())
            {
                i_arguments_source >> ',';
            }
        }
        (void)accept(spaces, i_arguments_source);

        if (m_function->qualified_return_type() == get_qualified_type<void>())
        {
            m_function->invoke(m_subobject, nullptr, arguments.data());
            return raw_ptr();
        }
        else
        {
            auto const value =
              m_dyn_value.manual_construct(m_function->qualified_return_type(), [&](void * i_dest) {
                  m_function->invoke(m_subobject, i_dest, arguments.data());
              });
            return raw_ptr(value, m_function->qualified_return_type());
        }
    }

    dyn_value invoke_function(const raw_ptr & i_target, char_reader & i_function_and_arguments)
    {
        auto const function_name = try_parse_identifier(i_function_and_arguments);
        if (function_name.empty())
        {
            except<std::logic_error>("Missing function name");
        }

        if (i_target.empty())
        {
            except<std::logic_error>("Missing function target");
        }

        (void)accept(spaces, i_function_and_arguments);
        if (!accept('(', i_function_and_arguments).has_value())
        {
            except<std::logic_error>("Missing '('");
        }

        dyn_value return_value;
        bool      found = false;
        for (auto const & function : inspect_functions(i_target))
        {
            if (function.name() == function_name)
            {
                return_value = function.invoke(i_function_and_arguments);
                found        = true;
                break;
            }
        }

        if (!found)
        {
            except<std::runtime_error>("Could not find the function ", function_name);
        }

        if (!accept(')', i_function_and_arguments).has_value())
        {
            except<std::logic_error>("Missing ')'");
        }

        return return_value;
    }

    dyn_value
      invoke_function(const raw_ptr & i_target, const string_view & i_function_and_arguments)
    {
        char_reader source(i_function_and_arguments);
        dyn_value   return_value = invoke_function(i_target, source);
        if (source.remaining_chars() != 0)
            throw parse_error::tailing_chars;
        return return_value;
    }

} // namespace edi
