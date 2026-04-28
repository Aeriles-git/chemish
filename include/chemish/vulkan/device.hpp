#pragma once

#include <vulkan/vulkan.hpp>

struct SDL_Window;

namespace chemish::vulkan {

class Device {
public:
  Device(SDL_Window *window);

  ~Device();

private:
  vk::Instance _instance = nullptr;
};

} // namespace chemish::vulkan
