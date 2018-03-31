
//   Copyright Giuseppe Campana (giu.campana@gmail.com) 2017-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "instance.h"

namespace vulkaninc
{
    Instance::Instance(char const * i_app_name, uint32_t i_app_version)
    {
#ifdef _WIN32
        char const * instance_extensions[] = {VK_KHR_SURFACE_EXTENSION_NAME,
                                              VK_KHR_WIN32_SURFACE_EXTENSION_NAME};
#else
        char const * instance_extensions[] = {VK_KHR_SURFACE_EXTENSION_NAME};
#endif

        // create the instance
        vk::ApplicationInfo const application_info{
          i_app_name, i_app_version, "silurian", 0, VK_API_VERSION_1_1};
        vk::InstanceCreateInfo const instance_info{{},
                                                   &application_info,
                                                   array_size_u32(static_configuration::layers),
                                                   static_configuration::layers,
                                                   array_size_u32(instance_extensions),
                                                   instance_extensions};
        m_instance = vk::createInstance(instance_info);

        // create the devices
        auto const devices = m_instance->enumeratePhysicalDevices();
        m_devices.reserve(devices.size());
        for (auto & device : devices)
        {
            m_devices.emplace_back(m_instance, device);
        }
    }

} // namespace vulkaninc

#include "vulkanic/device.h"
#include "vulkanic/surface.h"
#include "vulkanic/swapchain.h"

void f()
{
    const char           title[] = "abc";
    vulkaninc::Instance  instance("test", 1);
    vulkaninc::Surface   surface(instance.handle(), "test");
    auto const &         device = instance.devices()[0];
    vulkaninc::Swapchain swap_chain(surface.handle(), device.physical_device(), device.handle());
}
