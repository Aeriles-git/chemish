#include <chemish/rhi/vulkan/device.hpp>
#include <chemish/rhi/vulkan/swapchain.hpp>

namespace chemish::rhi::vulkan {

Swapchain::Swapchain(Device &dev) : device(dev) {
  VkPhysicalDevice physical = device.getPhysical();
  VkDevice logical = device.getLogical();
  VkSurfaceKHR surface = device.getSurface();

  VkSurfaceCapabilitiesKHR caps{};
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical, surface, &caps);

  if (caps.currentExtent.width != 0xFFFFFFFF) {
    extent = caps.currentExtent;
  } else {
    extent.width = 1280;
    extent.height = 720;
  }

  uint32_t formatCount = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physical, surface, &formatCount,
                                       nullptr);
  std::vector<VkSurfaceFormatKHR> formats(formatCount);
  vkGetPhysicalDeviceSurfaceFormatsKHR(physical, surface, &formatCount,
                                       formats.data());

  VkSurfaceFormatKHR chosen = formats[0];
  for (const auto &f : formats) {
    if (f.format == VK_FORMAT_B8G8R8A8_SRGB &&
        f.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
      chosen = f;
      break;
    }
  }
  format = chosen.format;

  uint32_t imageCount = caps.minImageCount + 1;
  if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount) {
    imageCount = caps.maxImageCount;
  }

  VkSwapchainCreateInfoKHR ci{};
  ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  ci.surface = surface;
  ci.minImageCount = imageCount;
  ci.imageFormat = chosen.format;
  ci.imageColorSpace = chosen.colorSpace;
  ci.imageExtent = extent;
  ci.imageArrayLayers = 1;
  ci.imageUsage =
      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  ci.preTransform = caps.currentTransform;
  ci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  ci.presentMode = VK_PRESENT_MODE_FIFO_KHR;
  ci.clipped = VK_TRUE;

  vkCreateSwapchainKHR(logical, &ci, nullptr, &handle);

  uint32_t actualCount = 0;
  vkGetSwapchainImagesKHR(logical, handle, &actualCount, nullptr);
  images.resize(actualCount);
  vkGetSwapchainImagesKHR(logical, handle, &actualCount, images.data());
}

Swapchain::~Swapchain() {
  vkDestroySwapchainKHR(device.getLogical(), handle, nullptr);
}

} // namespace chemish::rhi::vulkan
