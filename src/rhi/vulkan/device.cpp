#include <chemish/rhi/vulkan/device.hpp>

#include <chemish/debug.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <cstdio>
#include <cstdlib>
#include <vector>

namespace chemish::rhi::vulkan {

Device::Device(SDL_Window *win) : window(win) {
  // Instance
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "chemish";
  appInfo.apiVersion = VK_API_VERSION_1_4;

  Uint32 extCount = 0;
  const char *const *sdlExts = SDL_Vulkan_GetInstanceExtensions(&extCount);
  std::vector<const char *> extensions(sdlExts, sdlExts + extCount);
  extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  const char *layers[] = {chemish::kValidationLayerName};

  VkDebugUtilsMessengerCreateInfoEXT dbgInfo;
  chemish::fillDebugMessengerInfo(dbgInfo);

  VkInstanceCreateInfo instanceInfo{};
  instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceInfo.pNext = &dbgInfo;
  instanceInfo.pApplicationInfo = &appInfo;
  instanceInfo.enabledLayerCount = 1;
  instanceInfo.ppEnabledLayerNames = layers;
  instanceInfo.enabledExtensionCount = (uint32_t)extensions.size();
  instanceInfo.ppEnabledExtensionNames = extensions.data();

  if (vkCreateInstance(&instanceInfo, nullptr, &instance) != VK_SUCCESS) {
    std::fprintf(stderr, "vkCreateInstance failed\n");
    std::exit(1);
  }

  messenger = chemish::createDebugMessenger(instance);

  // Surface
  if (!SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface)) {
    std::fprintf(stderr, "SDL_Vulkan_CreateSurface failed: %s\n",
                 SDL_GetError());
    std::exit(1);
  }

  // Physical device
  uint32_t gpuCount = 0;
  vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);
  std::vector<VkPhysicalDevice> gpus(gpuCount);
  vkEnumeratePhysicalDevices(instance, &gpuCount, gpus.data());
  physical = gpus[0];

  // Queue family: graphics + presentation
  uint32_t qfCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physical, &qfCount, nullptr);
  std::vector<VkQueueFamilyProperties> qfs(qfCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physical, &qfCount, qfs.data());
  for (uint32_t i = 0; i < qfCount; i++) {
    VkBool32 canPresent = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(physical, i, surface, &canPresent);
    if ((qfs[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && canPresent) {
      queueFamily = i;
      break;
    }
  }

  // Features
  VkPhysicalDeviceVulkan11Features f11{};
  f11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
  f11.shaderDrawParameters = VK_TRUE;

  VkPhysicalDeviceVulkan13Features f13{};
  f13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
  f13.synchronization2 = VK_TRUE;
  f13.dynamicRendering = VK_TRUE;
  f13.pNext = &f11;

  // Logical device
  float priority = 1.0f;
  VkDeviceQueueCreateInfo qci{};
  qci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  qci.queueFamilyIndex = queueFamily;
  qci.queueCount = 1;
  qci.pQueuePriorities = &priority;

  const char *deviceExts[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  VkDeviceCreateInfo dci{};
  dci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  dci.pNext = &f13;
  dci.queueCreateInfoCount = 1;
  dci.pQueueCreateInfos = &qci;
  dci.enabledExtensionCount = 1;
  dci.ppEnabledExtensionNames = deviceExts;

  vkCreateDevice(physical, &dci, nullptr, &logical);
  vkGetDeviceQueue(logical, queueFamily, 0, &queue);
}

Device::~Device() {
  vkDestroyDevice(logical, nullptr);
  SDL_Vulkan_DestroySurface(instance, surface, nullptr);
  chemish::destroyDebugMessenger(instance, messenger);
  vkDestroyInstance(instance, nullptr);
}

} // namespace chemish::rhi::vulkan
