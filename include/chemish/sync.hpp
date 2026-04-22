#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace chemish {

struct FrameSync {
  VkFence inFlight = VK_NULL_HANDLE;           // CPU waits on this
  VkSemaphore renderFinished = VK_NULL_HANDLE; // GPU waits before presenting
};

FrameSync createFrameSync(VkDevice device);
void destroyFrameSync(VkDevice device, FrameSync &sync);

std::vector<VkSemaphore> createImageSemaphores(VkDevice device, uint32_t count);
void destroyImageSemaphores(VkDevice device,
                            std::vector<VkSemaphore> &semaphores);

} // namespace chemish
