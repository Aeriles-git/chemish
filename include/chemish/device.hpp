#pragma once

#include <vulkan/vulkan.h>

namespace chemish {

struct Device {
  VkPhysicalDevice physical = VK_NULL_HANDLE;
  VkDevice logical = VK_NULL_HANDLE;
  VkQueue queue = VK_NULL_HANDLE;
  uint32_t queueFamily = 0;
};

Device createDevice(VkInstance instance, VkSurfaceKHR surface);

void destroyDevice(Device &device);

} // namespace chemish
