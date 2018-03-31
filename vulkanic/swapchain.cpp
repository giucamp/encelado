
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "vulkanic/swapchain.h"
#include <algorithm>

namespace vulkaninc
{
    Swapchain::Swapchain(
      vk::SurfaceKHR i_surface, vk::PhysicalDevice i_physical_device, vk::Device i_logical_device)
    {
        auto const capabilities = i_physical_device.getSurfaceCapabilitiesKHR(i_surface);
        //auto const present_modes = i_physical_device.getSurfacePresentModesKHR(i_surface);

        // choose the format
        auto const formats = i_physical_device.getSurfaceFormatsKHR(i_surface);
        auto const format_it =
          std::find_if(formats.begin(), formats.end(), [](const vk::SurfaceFormatKHR & i_format) {
              return i_format.format == vk::Format::eB8G8R8A8Unorm;
          });
        check(format_it != formats.end(), "No B8G8R8A8Unorm surface format supported");

        vk::SwapchainCreateInfoKHR swapchain_info;
        swapchain_info.minImageCount    = capabilities.minImageCount;
        swapchain_info.surface          = i_surface;
        swapchain_info.imageFormat      = format_it->format;
        swapchain_info.imageColorSpace  = format_it->colorSpace;
        swapchain_info.presentMode      = vk::PresentModeKHR::eFifo;
        swapchain_info.imageArrayLayers = 1;
        swapchain_info.clipped          = true;
        swapchain_info.imageUsage       = vk::ImageUsageFlagBits::eColorAttachment;
        swapchain_info.imageSharingMode = vk::SharingMode::eExclusive;
        swapchain_info.preTransform     = capabilities.currentTransform;
        swapchain_info.compositeAlpha   = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        if (capabilities.currentExtent.width == 0xFFFFFFFF)
        {
            swapchain_info.imageExtent.width = clamp(
              uint32_t(800), capabilities.minImageExtent.width, capabilities.minImageExtent.height);
            swapchain_info.imageExtent.height = clamp(
              uint32_t(600),
              capabilities.minImageExtent.height,
              capabilities.minImageExtent.height);
        }
        else
        {
            swapchain_info.imageExtent = capabilities.currentExtent;
        }

        m_swapchain =
          SwapchainHandle{i_logical_device.createSwapchainKHR(swapchain_info), {i_logical_device}};
    }

} // namespace vulkaninc
