
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "vulkanic/vulkanic.h"
#include <memory>

namespace vulkaninc
{

    class Swapchain
    {
      public:
        Swapchain(
          vk::SurfaceKHR     i_surface,
          vk::PhysicalDevice i_physical_device,
          vk::Device         i_logical_device);

      private:
        SwapchainHandle m_swapchain;
    };

} // namespace vulkaninc
