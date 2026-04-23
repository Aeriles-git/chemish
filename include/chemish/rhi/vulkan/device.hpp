#pragma once

#include <chemish/rhi/device.hpp>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

struct SDL_Window;

namespace chemish::rhi::vulkan {

class Device : public rhi::Device {
public:
  Device(SDL_Window *window);
  ~Device() override;

  Device(const Device &) = delete;
  Device &operator=(const Device &) = delete;

  const VkInstance &getInstance() const { return instance; }
  const VkSurfaceKHR &getSurface() const { return surface; }
  const VkPhysicalDevice &getPhysical() const { return physical; }
  const VkDevice &getLogical() const { return logical; }
  const VkQueue &getQueue() const { return queue; }
  uint32_t getQueueFamily() const { return queueFamily; }
  const VmaAllocator &getAllocator() const { return allocator; }

private:
  VkInstance instance = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  VkPhysicalDevice physical = VK_NULL_HANDLE;
  VkDevice logical = VK_NULL_HANDLE;
  VkQueue queue = VK_NULL_HANDLE;
  uint32_t queueFamily = 0;
  VmaAllocator allocator = VK_NULL_HANDLE;

  SDL_Window *window = nullptr;
};

} // namespace chemish::rhi::vulkan
