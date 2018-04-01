
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "vulkanic/swapchain.h"
#include <algorithm>

namespace vulkaninc
{
    Swapchain::Swapchain(Surface * i_surface, Device * i_device)
    {
        auto const surface_handle  = i_surface->handle();
        auto const physical_device = i_device->physical_device();
        auto const logical_device  = i_device->handle();

        auto const capabilities = physical_device.getSurfaceCapabilitiesKHR(surface_handle);
        //auto const present_modes = physical_device.getSurfacePresentModesKHR(surface_handle);

        // choose the format
        auto const formats = physical_device.getSurfaceFormatsKHR(surface_handle);
        auto const format_it =
          std::find_if(formats.begin(), formats.end(), [](const vk::SurfaceFormatKHR & i_format) {
              return i_format.format == vk::Format::eB8G8R8A8Unorm;
          });
        check(format_it != formats.end(), "No B8G8R8A8Unorm surface format supported");

        vk::SwapchainCreateInfoKHR swapchain_info;
        swapchain_info.minImageCount    = capabilities.minImageCount;
        swapchain_info.surface          = surface_handle;
        swapchain_info.imageFormat      = format_it->format;
        swapchain_info.imageColorSpace  = format_it->colorSpace;
        swapchain_info.presentMode      = vk::PresentModeKHR::eFifo;
        swapchain_info.imageArrayLayers = 1;
        swapchain_info.clipped          = true;
        swapchain_info.imageUsage       = vk::ImageUsageFlagBits::eColorAttachment;
        swapchain_info.imageSharingMode = vk::SharingMode::eExclusive;
        swapchain_info.preTransform     = capabilities.currentTransform;
        swapchain_info.compositeAlpha   = vk::CompositeAlphaFlagBitsKHR::eOpaque;

        uint32_t queue_indices[2] = {i_device->render_queue_family_index(),
                                     i_device->present_queue_family_index()};
        if (queue_indices[0] != queue_indices[1])
        {
            swapchain_info.queueFamilyIndexCount = 2;
            swapchain_info.pQueueFamilyIndices   = queue_indices;
            swapchain_info.imageSharingMode      = vk::SharingMode::eConcurrent;
        }

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
          SwapchainHandle{logical_device.createSwapchainKHR(swapchain_info), {logical_device}};

        m_images = logical_device.getSwapchainImagesKHR(m_swapchain);
        m_image_views.reserve(m_images.size());
        for (auto & image : m_images)
        {
            vk::ImageViewCreateInfo image_view_info;
            image_view_info.image                           = image;
            image_view_info.viewType                        = vk::ImageViewType::e2D;
            image_view_info.format                          = swapchain_info.imageFormat;
            image_view_info.components.r                    = vk::ComponentSwizzle::eR;
            image_view_info.components.g                    = vk::ComponentSwizzle::eG;
            image_view_info.components.b                    = vk::ComponentSwizzle::eB;
            image_view_info.components.a                    = vk::ComponentSwizzle::eA;
            image_view_info.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
            image_view_info.subresourceRange.baseMipLevel   = 0;
            image_view_info.subresourceRange.levelCount     = 1;
            image_view_info.subresourceRange.baseArrayLayer = 0;
            image_view_info.subresourceRange.layerCount     = 1;
            m_image_views.push_back(
              ImageViewHandle{logical_device.createImageView(image_view_info), {logical_device}});
        }
    }

} // namespace vulkaninc
