
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "vulkanic/device.h"
#include "vulkanic/surface.h"
#include "vulkanic/vulkanic.h"
#include <memory>

namespace vulkaninc
{

    class Swapchain
    {
      public:
        Swapchain(Surface * i_surface, Device * i_device);

      private:
        SwapchainHandle              m_swapchain;
        std::vector<vk::Image>       m_images;
        std::vector<ImageViewHandle> m_image_views;
    };

} // namespace vulkaninc
