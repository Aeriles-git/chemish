#include <chemish/sync.hpp>

namespace chemish {

FrameSync createFrameSync(VkDevice device) {
  FrameSync sync{};

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  vkCreateFence(device, &fenceInfo, nullptr, &sync.inFlight);

  return sync;
}

void destroyFrameSync(VkDevice device, FrameSync &sync) {
  vkDestroyFence(device, sync.inFlight, nullptr);
}

std::vector<VkSemaphore> createImageSemaphores(VkDevice device,
                                               uint32_t count) {
  std::vector<VkSemaphore> semaphores(count);
  VkSemaphoreCreateInfo semInfo{};
  semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  for (auto &s : semaphores) {
    vkCreateSemaphore(device, &semInfo, nullptr, &s);
  }
  return semaphores;
}

void destroyImageSemaphores(VkDevice device,
                            std::vector<VkSemaphore> &semaphores) {
  for (auto &s : semaphores) {
    vkDestroySemaphore(device, s, nullptr);
  }
  semaphores.clear();
}

} // namespace chemish
