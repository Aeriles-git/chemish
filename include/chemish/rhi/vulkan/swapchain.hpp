#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace chemish::rhi::vulkan {

class Device;

class Swapchain {
public:
  Swapchain(Device &device);
  ~Swapchain();

  Swapchain(const Swapchain &) = delete;
  Swapchain &operator=(const Swapchain &) = delete;

  // Temporary accessors for the migration.
  VkSwapchainKHR getHandle() const { return handle; }
  VkFormat getFormat() const { return format; }
  VkExtent2D getExtent() const { return extent; }
  const std::vector<VkImage> &getImages() const { return images; }

private:
  Device &device; // reference, not ownership

  VkSwapchainKHR handle = VK_NULL_HANDLE;
  VkFormat format = VK_FORMAT_UNDEFINED;
  VkExtent2D extent{};
  std::vector<VkImage> images;
};

} // namespace chemish::rhi::vulkan
