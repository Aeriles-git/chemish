#pragma once

#include <vulkan/vulkan.h>

namespace chemish::rhi::vulkan {

class Device;

class Commands {
public:
  Commands(Device &device);
  ~Commands();

  Commands(const Commands &) = delete;
  Commands &operator=(const Commands &) = delete;

  const VkCommandBuffer &getBuffer() const { return buffer; }

private:
  Device &device;
  VkCommandPool pool = VK_NULL_HANDLE;
  VkCommandBuffer buffer = VK_NULL_HANDLE;
};

} // namespace chemish::rhi::vulkan
