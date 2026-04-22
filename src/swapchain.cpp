#include <chemish/swapchain.hpp>

#include <vector>

namespace chemish {

Swapchain createSwapchain(VkPhysicalDevice physical, VkDevice device,
                          VkSurfaceKHR surface) {
  Swapchain sc{};

  // Query what the surface can do.
  VkSurfaceCapabilitiesKHR caps{};
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical, surface, &caps);

  if (caps.currentExtent.width != 0xFFFFFFFF) {
    sc.extent = caps.currentExtent;
  } else {
    sc.extent.width = 1280;
    sc.extent.height = 720;
  }

  // Pick a format.
  uint32_t formatCount = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physical, surface, &formatCount,
                                       nullptr);
  std::vector<VkSurfaceFormatKHR> formats(formatCount);
  vkGetPhysicalDeviceSurfaceFormatsKHR(physical, surface, &formatCount,
                                       formats.data());

  VkSurfaceFormatKHR chosenFormat = formats[0];
  for (const auto &f : formats) {
    if (f.format == VK_FORMAT_B8G8R8A8_SRGB &&
        f.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
      chosenFormat = f;
      break;
    }
  }
  sc.format = chosenFormat.format;

  // Pick image count (one more than the minimum, capped at max).
  uint32_t imageCount = caps.minImageCount + 1;
  if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount) {
    imageCount = caps.maxImageCount;
  }

  // Create the swapchain.
  VkSwapchainCreateInfoKHR ci{};
  ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  ci.surface = surface;
  ci.minImageCount = imageCount;
  ci.imageFormat = chosenFormat.format;
  ci.imageColorSpace = chosenFormat.colorSpace;
  ci.imageExtent = sc.extent;
  ci.imageArrayLayers = 1;
  ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  ci.preTransform = caps.currentTransform;
  ci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  ci.presentMode = VK_PRESENT_MODE_FIFO_KHR;
  ci.clipped = VK_TRUE;

  vkCreateSwapchainKHR(device, &ci, nullptr, &sc.handle);

  // Retrieve the images the swapchain created.
  uint32_t actualCount = 0;
  vkGetSwapchainImagesKHR(device, sc.handle, &actualCount, nullptr);
  sc.images.resize(actualCount);
  vkGetSwapchainImagesKHR(device, sc.handle, &actualCount, sc.images.data());

  return sc;
}

void destroySwapchain(VkDevice device, Swapchain &sc) {
  vkDestroySwapchainKHR(device, sc.handle, nullptr);
}

} // namespace chemish
