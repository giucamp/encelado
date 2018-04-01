
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "vulkanic/vulkanic.h"
#include <utility>
#include <vector>

namespace vulkaninc
{
    class CommandQueue
    {
      public:
        CommandQueue(vk::Device i_device, uint32_t i_family_index);

      private:
        CommandPoolHandle                m_commnand_pool;
        std::vector<CommandBufferHandle> m_commnand_buffers;
    };

    class Device
    {
      public:
        Device(
          vk::Instance               i_vk_instance,
          const vk::PhysicalDevice & i_physical_device,
          vk::SurfaceKHR             i_surface);

        vk::PhysicalDevice const physical_device() const noexcept { return m_physical_device; }

        vk::Instance vk_instance() const noexcept { return m_vk_instance; }

        vk::Device handle() const noexcept { return m_device; }

        uint32_t render_queue_family_index() const noexcept { return m_render_queue_family_index; }

        uint32_t present_queue_family_index() const noexcept
        {
            return m_present_queue_family_index;
        }

      private:
        std::vector<bool> get_surface_suppports(vk::SurfaceKHR i_surface) const;

      private:
        DeviceHandle                           m_device;
        std::vector<CommandQueue>              m_command_queues;
        uint32_t                               m_render_queue_family_index  = 0;
        uint32_t                               m_present_queue_family_index = 0;
        vk::PhysicalDevice const               m_physical_device;
        vk::Instance const                     m_vk_instance;
        std::vector<vk::QueueFamilyProperties> m_queue_families;
    };

} // namespace vulkaninc
