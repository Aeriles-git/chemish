#pragma once

#include <chemish/rhi/buffer.hpp>
#include <chemish/rhi/device.hpp>
#include <chemish/rhi/shader.hpp>

#include <memory>
#include <string>

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

  // rhi::Device implementation
  std::unique_ptr<rhi::Buffer>
  createVertexBuffer(size_t size, const void *initialData) override;
  std::unique_ptr<rhi::Shader>
  createShader(const std::string &spvPath) override;

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

std::unique_ptr<Device> createVulkanDevice(struct SDL_Window *window);

} // namespace chemish::rhi::vulkan
