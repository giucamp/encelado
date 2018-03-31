
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "device.h"

namespace vulkaninc
{
    Device::Device(vk::Instance i_vk_instance, const vk::PhysicalDevice & i_physical_device)
        : m_physical_device(i_physical_device), m_vk_instance(i_vk_instance)
    {
        char const * device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        // create just a queue for the graphics
        m_command_queues.reserve(1);
        float priorities[] = {0.f};

        vk::DeviceQueueCreateInfo queue_info;
        queue_info.setPQueuePriorities(priorities);
        queue_info.setQueueCount(1);
        auto const queue_properties      = i_physical_device.getQueueFamilyProperties();
        auto const queue_properties_size = static_cast<uint32_t>(queue_properties.size());
        uint32_t   queue_index           = 0;
        for (; queue_index < queue_properties_size; queue_index++)
        {
            if (queue_properties[queue_index].queueFlags & vk::QueueFlagBits::eGraphics)
            {
                queue_info.setQueueFamilyIndex(queue_index);
                break;
            }
        }
        if (queue_index >= queue_properties_size)
        {
            throw std::runtime_error("vulkaninc: could not find a queue in the device");
        }
        vk::DeviceCreateInfo device_info;
        device_info.setQueueCreateInfoCount(1);
        device_info.setPQueueCreateInfos(&queue_info);
        device_info.setPpEnabledExtensionNames(device_extensions);
        device_info.setEnabledExtensionCount(array_size_u32(device_extensions));
        m_device = i_physical_device.createDevice(device_info);
        m_command_queues.emplace_back(m_device, queue_index);
    }

    CommandQueue::CommandQueue(vk::Device i_device, uint32_t i_queue_index)
    {
        vk::CommandPoolCreateInfo command_pool_info;
        command_pool_info.setQueueFamilyIndex(i_queue_index);
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
