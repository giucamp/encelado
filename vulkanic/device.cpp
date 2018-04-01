
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "device.h"
#include <limits>

namespace vulkaninc
{
    std::vector<bool> Device::get_surface_suppports(vk::SurfaceKHR i_surface) const
    {
        auto const queue_properties_size = static_cast<uint32_t>(m_queue_families.size());

        std::vector<bool> surface_suppports;
        surface_suppports.reserve(m_queue_families.size());
        for (uint32_t queue_index = 0; queue_index < queue_properties_size; queue_index++)
        {
            surface_suppports.push_back(
              m_physical_device.getSurfaceSupportKHR(
                static_cast<uint32_t>(queue_index), i_surface) == VK_TRUE);
        }
        return surface_suppports;
    }


    Device::Device(
      vk::Instance               i_vk_instance,
      const vk::PhysicalDevice & i_physical_device,
      vk::SurfaceKHR             i_surface)
        : m_physical_device(i_physical_device), m_vk_instance(i_vk_instance)
    {
        m_queue_families                 = i_physical_device.getQueueFamilyProperties();
        auto const queue_properties_size = static_cast<uint32_t>(m_queue_families.size());
        auto const surface_suppports     = get_surface_suppports(i_surface);

        // create just a queue for the graphics
        m_command_queues.reserve(1);
        float priorities[] = {0.f};

        auto const invalid_index     = (std::numeric_limits<uint32_t>::max)();
        m_render_queue_family_index  = invalid_index;
        m_present_queue_family_index = invalid_index;

        for (uint32_t queue_index = 0; queue_index < queue_properties_size; queue_index++)
        {
            if (
              surface_suppports[queue_index] &&
              (m_queue_families[queue_index].queueFlags & vk::QueueFlagBits::eGraphics) !=
                vk::QueueFlagBits{})
            {
                m_render_queue_family_index  = queue_index;
                m_present_queue_family_index = queue_index;
                break;
            }
        }

        for (uint32_t queue_index = 0; queue_index < queue_properties_size; queue_index++)
        {
            if (m_present_queue_family_index == invalid_index && surface_suppports[queue_index])
            {
                m_present_queue_family_index = queue_index;
            }

            if (
              m_render_queue_family_index == invalid_index &&
              (m_queue_families[queue_index].queueFlags & vk::QueueFlagBits::eGraphics) !=
                vk::QueueFlagBits{})
            {
                m_render_queue_family_index = queue_index;
            }
        }

        check(
          m_render_queue_family_index != invalid_index,
          "vulkaninc: could not find a render-enabled queue family in the device");
        check(
          m_present_queue_family_index != invalid_index,
          "vulkaninc: could not find a present-enabled queue family in the device");

        uint32_t                  queue_info_count = 1;
        vk::DeviceQueueCreateInfo queue_infos[2];
        queue_infos[0].setPQueuePriorities(priorities);
        queue_infos[0].setQueueCount(1);
        queue_infos[0].setQueueFamilyIndex(m_render_queue_family_index);
        if (m_render_queue_family_index != m_present_queue_family_index)
        {
            queue_info_count = 2;
            queue_infos[0].setPQueuePriorities(priorities);
            queue_infos[0].setQueueCount(1);
            queue_infos[0].setQueueFamilyIndex(m_present_queue_family_index);
        }

        char const *         device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
        vk::DeviceCreateInfo device_info;
        device_info.setQueueCreateInfoCount(queue_info_count);
        device_info.setPQueueCreateInfos(queue_infos);
        device_info.setPpEnabledExtensionNames(device_extensions);
        device_info.setEnabledExtensionCount(array_size_u32(device_extensions));
        m_device = i_physical_device.createDevice(device_info);
        m_command_queues.emplace_back(m_device, m_render_queue_family_index);
    }

    CommandQueue::CommandQueue(vk::Device i_device, uint32_t i_family_index)
    {
        vk::CommandPoolCreateInfo command_pool_info;
        command_pool_info.setQueueFamilyIndex(i_family_index);
        m_commnand_pool = CommandPoolHandle(
          i_device.createCommandPool(command_pool_info),
          handle_deleters::CommandPoolDeleter{i_device});

        // create a command buffer
        vk::CommandBufferAllocateInfo command_buffer_info;
        command_buffer_info.setCommandBufferCount(1);
        command_buffer_info.setLevel(vk::CommandBufferLevel::ePrimary);
        command_buffer_info.setCommandPool(m_commnand_pool);
        m_commnand_buffers.reserve(1);
        auto const command_buffers = i_device.allocateCommandBuffers(command_buffer_info);

        for (const auto & command_buffer : command_buffers)
        {
            m_commnand_buffers.emplace_back(
              command_buffer, handle_deleters::CommandBufferDeleter{i_device, m_commnand_pool});
        }
    }

} // namespace vulkaninc
