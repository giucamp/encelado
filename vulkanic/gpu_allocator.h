
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include "vulkanic/vulkanic.h"
// this line prevents reordering, vk_mem_alloc.h must be included, after vulkanic.h
#include "vulkanic/vk_mem_alloc.h"
#include <cstddef>
#include <utility>

namespace vulkaninc
{
    namespace handle_deleters
    {
        struct AllocatorDeleter
        {
            void operator()(VmaAllocator & i_obj) { vmaDestroyAllocator(i_obj); }
        };

        struct AllocationDeleter
        {
            VmaAllocator m_allocator;

            void operator()(VmaAllocation & i_obj) { vmaFreeMemory(m_allocator, i_obj); }
        };

    } // namespace handle_deleters

    using AllocatorHandle = Handle<VmaAllocator, handle_deleters::AllocatorDeleter>;

    using AllocationHandle = Handle<VmaAllocation, handle_deleters::AllocationDeleter>;

    class Image
    {
      public:
        Image() noexcept {}

        Image(
          VkDevice i_device, VkImage i_image, VmaAllocator i_allocator, VmaAllocation i_allocation);

        VmaAllocationInfo allocation_info() const;

      private:
        ImageHandle      m_image;
        AllocationHandle m_allocation;
    };

    class Buffer
    {
      public:
        Buffer() noexcept {}

        Buffer(
          VkDevice      i_device,
          VkBuffer      i_image,
          VmaAllocator  i_allocator,
          VmaAllocation i_allocation);

        VmaAllocationInfo allocation_info() const;

      private:
        BufferHandle     m_buffer;
        AllocationHandle m_allocation;
    };

    class GpuAllocator
    {
      public:
        GpuAllocator() = default;

        void init(vk::PhysicalDevice i_physical_device, vk::Device i_logical_device);

        Image create_image(
          vk::ImageCreateInfo const &     i_image_info,
          VmaAllocationCreateInfo const & i_allocation_info);

        Buffer create_buffer(
          vk::BufferCreateInfo const &    i_buffer_info,
          VmaAllocationCreateInfo const & i_allocation_info);

      private:
        VkDevice        m_device{};
        AllocatorHandle m_allocator;
    };

} // namespace vulkaninc
