#include <chemish/commands.hpp>

namespace chemish {

Commands createCommands(VkDevice device, uint32_t queueFamily) {
  Commands commands{};

  // Pool: an allocator for command buffers tied to one queue family.
  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = queueFamily;
  vkCreateCommandPool(device, &poolInfo, nullptr, &commands.pool);

  // Allocate one primary command buffer from the pool.
  VkCommandBufferAllocateInfo bufInfo{};
  bufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  bufInfo.commandPool = commands.pool;
  bufInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  bufInfo.commandBufferCount = 1;
  vkAllocateCommandBuffers(device, &bufInfo, &commands.buffer);

  return commands;
}

void destroyCommands(VkDevice device, Commands &commands) {
  // Destroying the pool frees all buffers allocated from it.
  vkDestroyCommandPool(device, commands.pool, nullptr);
}

} // namespace chemish
