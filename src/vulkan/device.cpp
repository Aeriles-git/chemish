#include <chemish/vulkan/device.hpp>
#include <vulkan/vulkan.hpp>

#include <SDL3/SDL_vulkan.h>
#include <vector>

namespace chemish::vulkan {

Device::Device(SDL_Window *window) {
  constexpr vk::ApplicationInfo appInfo{.pApplicationName = "chemish",
                                        .apiVersion = vk::ApiVersion14};

  uint32_t extCount = 0;
  const char *const *exts = SDL_Vulkan_GetInstanceExtensions(&extCount);
  std::vector<const char *> extensions(exts, exts + extCount);

  vk::InstanceCreateInfo info{.pApplicationInfo = &appInfo,
                              .enabledExtensionCount =
                                  (uint32_t)extensions.size(),
                              .ppEnabledExtensionNames = extensions.data()};

  _instance = vk::createInstance(info);
};

Device::~Device() { _instance.destroy(); }

} // namespace chemish::vulkan
