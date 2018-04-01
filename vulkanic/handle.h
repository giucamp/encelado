
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <cstddef>
#include <utility>

namespace vulkaninc
{
    template <typename TYPE, typename DELETER> class Handle : private DELETER
    {
      public:
        Handle() noexcept {}

        Handle(const Handle &) = delete;

        Handle & operator=(const Handle &) = delete;

        Handle(const TYPE & i_handle) noexcept : m_handle(i_handle) {}

        Handle(std::nullptr_t) noexcept {}

        Handle(const TYPE & i_handle, const DELETER & i_deleter) noexcept
            : DELETER(i_deleter), m_handle(i_handle)
        {
        }

        Handle(const TYPE & i_handle, DELETER && i_deleter) noexcept
            : DELETER(std::move(i_deleter)), m_handle(i_handle)
        {
        }

        Handle(Handle && i_source) noexcept
            : DELETER(std::move(static_cast<DELETER &>(i_source))), m_handle(i_source.m_handle)
        {
            i_source.m_handle = nullptr;
        }

        Handle & operator=(Handle && i_source) noexcept
        {
            static_cast<DELETER &>(*this) = std::move(static_cast<DELETER &>(i_source));
            m_handle                      = i_source.m_handle;
            i_source.m_handle             = nullptr;
            return *this;
        }

        ~Handle()
        {
            if (m_handle)
                this->operator()(m_handle);
        }

        explicit operator bool() const noexcept { return static_cast<bool>(m_handle); }

        operator const TYPE &() const noexcept { return m_handle; }

        const TYPE * operator->() const noexcept { return &m_handle; }

        TYPE * operator->() noexcept { return &m_handle; }

        friend void swap(Handle & i_first, Handle & i_second) noexcept
        {
            using namespace std;
            swap(static_cast<DELETER &>(i_first), static_cast<DELETER &>(i_second));
            swap(i_first.m_handle, i_second.m_handle);
        }

      private:
        TYPE m_handle;
    };


} // namespace vulkaninc
