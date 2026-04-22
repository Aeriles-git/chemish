#include <cstdio>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <chemish/debug.hpp>
#include <chemish/device.hpp>
#include <chemish/swapchain.hpp>

int main() {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window =
      SDL_CreateWindow("chemish", 1280, 720, SDL_WINDOW_VULKAN);

  // Vulkan instance
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

  VkInstance instance = VK_NULL_HANDLE;
  if (vkCreateInstance(&instanceInfo, nullptr, &instance) != VK_SUCCESS) {
    std::fprintf(stderr, "vkCreateInstance failed\n");
    return 1;
  }

  VkDebugUtilsMessengerEXT messenger = chemish::createDebugMessenger(instance);

  // Vulkan surface
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  if (!SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface)) {
    std::fprintf(stderr, "SDL_Vulkan_CreateSurface failed: %s\n",
                 SDL_GetError());
    return 1;
  }

  chemish::Device device = chemish::createDevice(instance, surface);
  chemish::Swapchain swapchain =
      chemish::createSwapchain(device.physical, device.logical, surface);

  bool running = true;
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT)
        running = false;
      if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
        running = false;
    }
  }

  // Destroy
  chemish::destroySwapchain(device.logical, swapchain);
  chemish::destroyDevice(device);
  SDL_Vulkan_DestroySurface(instance, surface, nullptr);
  chemish::destroyDebugMessenger(instance, messenger);

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
