#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace chemish::rhi::vulkan {

class Device;

// Per-frame synchronization: just a fence for now.
// CPU waits on this to know the previous frame's GPU work is done.
class FrameSync {
public:
  FrameSync(Device &device);
  ~FrameSync();

  FrameSync(const FrameSync &) = delete;
  FrameSync &operator=(const FrameSync &) = delete;

  const VkFence &getFence() const { return fence; }

private:
  Device &device;
  VkFence fence = VK_NULL_HANDLE;
};

// N binary semaphores, one per swapchain image.
class ImageSemaphores {
public:
  ImageSemaphores(Device &device, uint32_t count);
  ~ImageSemaphores();

  ImageSemaphores(const ImageSemaphores &) = delete;
  ImageSemaphores &operator=(const ImageSemaphores &) = delete;

  const VkSemaphore &operator[](size_t i) const { return semaphores[i]; }
  size_t size() const { return semaphores.size(); }

private:
  Device &device;
  std::vector<VkSemaphore> semaphores;
};

} // namespace chemish::rhi::vulkan
