#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace chemish {

struct Swapchain {
  VkSwapchainKHR handle = VK_NULL_HANDLE;
  VkFormat format = VK_FORMAT_UNDEFINED;
  VkExtent2D extent{};
  std::vector<VkImage> images;
};

Swapchain createSwapchain(VkPhysicalDevice physical, VkDevice device,
                          VkSurfaceKHR surface);
void destroySwapchain(VkDevice device, Swapchain &sc);

} // namespace chemish
