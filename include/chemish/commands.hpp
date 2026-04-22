#pragma once

#include <vulkan/vulkan.h>

namespace chemish {

struct Commands {
  VkCommandPool pool = VK_NULL_HANDLE;
  VkCommandBuffer buffer = VK_NULL_HANDLE;
};

Commands createCommands(VkDevice device, uint32_t queueFamily);
void destroyCommands(VkDevice device, Commands &commands);

} // namespace chemish
