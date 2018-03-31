
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

//#define VULKAN_HPP_TYPESAFE_CONVERSION 1, for 32-bits, see https://github.com/KhronosGroup/Vulkan-Hpp
#include "vulkanic/handle.h"
#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan.hpp>

namespace vulkaninc
{
    namespace handle_deleters
    {
        struct InstanceDeleter
        {
            void operator()(vk::Instance & i_obj) { i_obj.destroy(); }
        };

        struct DeviceDeleter
        {
            void operator()(vk::Device & i_obj) { i_obj.destroy(); }
        };

        struct CommandPoolDeleter
        {
            vk::Device m_device;

            void operator()(vk::CommandPool & i_obj) { m_device.destroyCommandPool(i_obj); }
        };

        struct CommandBufferDeleter
        {
            vk::Device      m_device;
            vk::CommandPool m_command_pool;

            void operator()(vk::CommandBuffer & i_obj)
            {
                m_device.freeCommandBuffers(m_command_pool, 1, &i_obj);
            }
        };

        struct SurfaceDeleter
        {
            vk::Instance m_instance;

            void operator()(vk::SurfaceKHR & i_obj) { m_instance.destroySurfaceKHR(i_obj); }
        };

        struct SwapchainDeleter
        {
            vk::Device m_device;

            void operator()(vk::SwapchainKHR & i_obj) { m_device.destroySwapchainKHR(i_obj); }
        };

    } // namespace handle_deleters

    using InstanceHandle = Handle<vk::Instance, handle_deleters::InstanceDeleter>;

    using DeviceHandle = Handle<vk::Device, handle_deleters::DeviceDeleter>;

    using CommandPoolHandle = Handle<vk::CommandPool, handle_deleters::CommandPoolDeleter>;

    using CommandBufferHandle = Handle<vk::CommandBuffer, handle_deleters::CommandBufferDeleter>;

    using SurfaceHandle = Handle<vk::SurfaceKHR, handle_deleters::SurfaceDeleter>;

    using SwapchainHandle = Handle<vk::SwapchainKHR, handle_deleters::SwapchainDeleter>;

    template <typename TYPE, size_t COUNT> constexpr size_t array_size(TYPE (&)[COUNT])
    {
        return COUNT;
    }

    template <typename TYPE, size_t COUNT> constexpr uint32_t array_size_u32(TYPE (&)[COUNT])
    {
        return COUNT;
    }

    inline void check(bool i_value, const char * i_message)
    {
        if (!i_value)
            throw std::runtime_error(i_message);
    }

    template <typename TYPE>
    constexpr TYPE clamp(TYPE const & i_value, TYPE const & i_upper, TYPE const & i_lower)
    {
        return i_value < i_lower ? i_lower : (i_value > i_upper ? i_upper : i_value);
    }

} // namespace vulkaninc
