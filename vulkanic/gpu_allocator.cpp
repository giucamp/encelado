
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "vulkanic/gpu_allocator.h"
#define VMA_IMPLEMENTATION
#include "vulkanic/vk_mem_alloc.h"

namespace vulkaninc
{
    Image::Image(
      VkDevice i_device, VkImage i_image, VmaAllocator i_allocator, VmaAllocation i_allocation)
        : m_image(i_image, handle_deleters::ImageDeleter{i_device}),
          m_allocation(i_allocation, handle_deleters::AllocationDeleter{i_allocator})
    {
    }

    VmaAllocationInfo Image::allocation_info() const
    {
        VmaAllocationInfo result;
        vmaGetAllocationInfo(m_allocation.deleter().m_allocator, m_allocation, &result);
        return result;
    }

    Buffer::Buffer(
      VkDevice i_device, VkBuffer i_buffer, VmaAllocator i_allocator, VmaAllocation i_allocation)
        : m_buffer(i_buffer, handle_deleters::BufferDeleter{i_device}),
          m_allocation(i_allocation, handle_deleters::AllocationDeleter{i_allocator})
    {
    }

    VmaAllocationInfo Buffer::allocation_info() const
    {
        VmaAllocationInfo result;
        vmaGetAllocationInfo(m_allocation.deleter().m_allocator, m_allocation, &result);
        return result;
    }

    void GpuAllocator::init(vk::PhysicalDevice i_physical_device, vk::Device i_logical_device)
    {
        // https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/

        m_device                              = i_logical_device;
        VmaAllocatorCreateInfo allocator_info = {};
        allocator_info.physicalDevice         = i_physical_device;
        allocator_info.device                 = i_logical_device;
        VmaAllocator allocator;
        auto const   result = vmaCreateAllocator(&allocator_info, &allocator);
        check(result, "vmaCreateAllocator failed");
        m_allocator = allocator;
    }

    Image GpuAllocator::create_image(
      vk::ImageCreateInfo const & i_image_info, VmaAllocationCreateInfo const & i_allocation_info)
    {
        VkImage           image{};
        VmaAllocation     allocation{};
        VmaAllocationInfo allocation_info{};

        VkImageCreateInfo const & image_info = i_image_info;
        auto const                result     = vmaCreateImage(
          m_allocator, &image_info, &i_allocation_info, &image, &allocation, &allocation_info);
        check(result, "vmaCreateImage failed");

        return Image{m_device, image, m_allocator, allocation};
    }

    Buffer GpuAllocator::create_buffer(
      vk::BufferCreateInfo const & i_buffer_info, VmaAllocationCreateInfo const & i_allocation_info)
    {
        VkBuffer          buffer{};
        VmaAllocation     allocation{};
        VmaAllocationInfo allocation_info{};

        VkBufferCreateInfo const & buffer_info = i_buffer_info;
        auto const                 result      = vmaCreateBuffer(
          m_allocator, &buffer_info, &i_allocation_info, &buffer, &allocation, &allocation_info);
        check(result, "vmaCreateImage failed");

        return Buffer{m_device, buffer, m_allocator, allocation};
    }

} // namespace vulkaninc
