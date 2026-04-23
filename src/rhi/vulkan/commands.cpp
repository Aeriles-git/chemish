#include <chemish/rhi/vulkan/commands.hpp>
#include <chemish/rhi/vulkan/device.hpp>

namespace chemish::rhi::vulkan {

Commands::Commands(Device &dev) : device(dev) {
  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = device.getQueueFamily();
  vkCreateCommandPool(device.getLogical(), &poolInfo, nullptr, &pool);

  VkCommandBufferAllocateInfo bufInfo{};
  bufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  bufInfo.commandPool = pool;
  bufInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  bufInfo.commandBufferCount = 1;
  vkAllocateCommandBuffers(device.getLogical(), &bufInfo, &buffer);
}

Commands::~Commands() {
  vkDestroyCommandPool(device.getLogical(), pool, nullptr);
}

} // namespace chemish::rhi::vulkan
