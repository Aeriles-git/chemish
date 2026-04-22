#include <chemish/device.hpp>

#include <cstdio>
#include <vector>

namespace chemish {

Device createDevice(VkInstance instance, VkSurfaceKHR surface) {
  Device device{};

  // Pick first GPU.
  uint32_t gpuCount = 0;
  vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);
  std::vector<VkPhysicalDevice> gpus(gpuCount);
  vkEnumeratePhysicalDevices(instance, &gpuCount, gpus.data());
  device.physical = gpus[0];

  // Find a queue family that supports graphics AND presenting to our surface.
  uint32_t qfCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device.physical, &qfCount, nullptr);
  std::vector<VkQueueFamilyProperties> qfs(qfCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device.physical, &qfCount,
                                           qfs.data());

  for (uint32_t i = 0; i < qfCount; i++) {
    VkBool32 canPresent = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(device.physical, i, surface,
                                         &canPresent);
    if ((qfs[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && canPresent) {
      device.queueFamily = i;
      break;
    }
  }

  VkPhysicalDeviceVulkan13Features f13{};
  f13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
  f13.synchronization2 = VK_TRUE;
  f13.dynamicRendering = VK_TRUE;

  // Create the logical device with one queue and the swapchain extension.
  float priority = 1.0f;
  VkDeviceQueueCreateInfo qci{};
  qci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  qci.queueFamilyIndex = device.queueFamily;
  qci.queueCount = 1;
  qci.pQueuePriorities = &priority;

  const char *deviceExts[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  VkDeviceCreateInfo dci{};
  dci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  dci.queueCreateInfoCount = 1;
  dci.pQueueCreateInfos = &qci;
  dci.enabledExtensionCount = 1;
  dci.ppEnabledExtensionNames = deviceExts;
  dci.pNext = &f13;

  vkCreateDevice(device.physical, &dci, nullptr, &device.logical);
  vkGetDeviceQueue(device.logical, device.queueFamily, 0, &device.queue);
  return device;
}

void destroyDevice(Device &device) { vkDestroyDevice(device.logical, nullptr); }

} // namespace chemish
