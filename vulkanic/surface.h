
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "vulkanic/vulkanic.h"
#include <memory>

namespace vulkaninc
{
    class Surface
    {
      public:
        Surface(vk::Instance i_instance, const char * i_title);

        ~Surface();

        vk::SurfaceKHR handle() const noexcept { return m_surface; }

        /** Returns the sie of the window in */
        vk::Extent2D internal_extent() const noexcept;

      private:
        struct NativeWindow;
        std::unique_ptr<NativeWindow> m_native_window;
        SurfaceHandle                 m_surface;
    };

} // namespace vulkaninc
