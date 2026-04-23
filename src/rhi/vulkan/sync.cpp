#include <chemish/rhi/vulkan/device.hpp>
#include <chemish/rhi/vulkan/sync.hpp>

namespace chemish::rhi::vulkan {

FrameSync::FrameSync(Device &dev) : device(dev) {
  VkFenceCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  vkCreateFence(device.getLogical(), &info, nullptr, &fence);
}

FrameSync::~FrameSync() { vkDestroyFence(device.getLogical(), fence, nullptr); }

ImageSemaphores::ImageSemaphores(Device &dev, uint32_t count)
    : device(dev), semaphores(count) {
  VkSemaphoreCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  for (auto &s : semaphores) {
    vkCreateSemaphore(device.getLogical(), &info, nullptr, &s);
  }
}

ImageSemaphores::~ImageSemaphores() {
  for (auto &s : semaphores) {
    vkDestroySemaphore(device.getLogical(), s, nullptr);
  }
}

} // namespace chemish::rhi::vulkan
