#pragma once

#include <vulkan/vulkan.h>

namespace chemish {

struct FrameSync {
  VkFence inFlight = VK_NULL_HANDLE;           // CPU waits on this
  VkSemaphore imageAvailable = VK_NULL_HANDLE; // GPU waits before drawing
  VkSemaphore renderFinished = VK_NULL_HANDLE; // GPU waits before presenting
};

FrameSync createFrameSync(VkDevice device);
void destroyFrameSync(VkDevice device, FrameSync &sync);

} // namespace chemish
