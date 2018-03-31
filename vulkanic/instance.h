
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
    struct static_configuration
    {
        static constexpr char const * layers[] = {"VK_LAYER_LUNARG_api_dump",
                                                  "VK_LAYER_LUNARG_assistant_layer",
                                                  "VK_LAYER_LUNARG_core_validation",
                                                  "VK_LAYER_LUNARG_monitor",
                                                  "VK_LAYER_LUNARG_object_tracker",
                                                  "VK_LAYER_LUNARG_parameter_validation",
                                                  "VK_LAYER_GOOGLE_threading",
                                                  "VK_LAYER_GOOGLE_unique_objects"};
    };

    class Instance
    {
      public:
        Instance(char const * i_app_name, uint32_t i_app_version);

        vk::Instance handle() const noexcept { return m_instance; }

        std::vector<vk::PhysicalDevice> physical_devices() const;

      private:
        InstanceHandle m_instance;
    };

} // namespace vulkaninc
