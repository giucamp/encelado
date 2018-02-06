
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once


namespace edi
{
    namespace detail
    {
        // StaticEnum
        template <typename ENUM_TYPE, size_t NAME_LENGTH, size_t MEMBER_COUNT> class StaticEnum
        {
          public:
            using underlying_type = std::underlying_type_t<ENUM_TYPE>;

            constexpr StaticEnum(
              const array<char, NAME_LENGTH> &                        i_name,
              array<enum_member<underlying_type>, MEMBER_COUNT> const i_members)
                : m_name(i_name), m_members(i_members),
                  m_enum(
                    m_name.data(),
                    sizeof(ENUM_TYPE),
                    alignof(ENUM_TYPE),
                    special_functions::template make<ENUM_TYPE>(),
                    m_members)
            {
            }

            constexpr const enum_type<underlying_type> & get_class() const noexcept
            {
                return m_enum;
            }

          private:
            array<char, NAME_LENGTH>                                m_name;
            array<enum_member<underlying_type>, MEMBER_COUNT> const m_members;
            enum_type<underlying_type> const                        m_enum;
        };

    } // namespace detail

    template <typename ENUM_TYPE, size_t NAME_SIZE, size_t MEMBER_COUNT = 0>
    constexpr auto make_enum(
      const char (&i_name)[NAME_SIZE],
      array<enum_member<std::underlying_type_t<ENUM_TYPE>>, MEMBER_COUNT> const & i_members =
        array<enum_member<std::underlying_type_t<ENUM_TYPE>>, 0>{})
    {
        array<char, NAME_SIZE> name{};
        to_chars(name.data(), NAME_SIZE, i_name);
        return detail::StaticEnum<ENUM_TYPE, NAME_SIZE, MEMBER_COUNT>(name, i_members);
    }

    template <typename ENUM_TYPE>
    constexpr auto make_enum_member(const char * i_name, ENUM_TYPE i_value)
    {
        return enum_member<std::underlying_type_t<ENUM_TYPE>>(
          i_name, static_cast<std::underlying_type_t<ENUM_TYPE>>(i_value));
    }

} // namespace edi
