#include <chemish/sync.hpp>

namespace chemish {

FrameSync createFrameSync(VkDevice device) {
  FrameSync sync{};

  // Fence starts signaled so the first frame doesn't deadlock on wait.
  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  vkCreateFence(device, &fenceInfo, nullptr, &sync.inFlight);

  VkSemaphoreCreateInfo semInfo{};
  semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  vkCreateSemaphore(device, &semInfo, nullptr, &sync.imageAvailable);
  vkCreateSemaphore(device, &semInfo, nullptr, &sync.renderFinished);

  return sync;
}

void destroyFrameSync(VkDevice device, FrameSync &sync) {
  vkDestroyFence(device, sync.inFlight, nullptr);
  vkDestroySemaphore(device, sync.imageAvailable, nullptr);
  vkDestroySemaphore(device, sync.renderFinished, nullptr);
}

} // namespace chemish
