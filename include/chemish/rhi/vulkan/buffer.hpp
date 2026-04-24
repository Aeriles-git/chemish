#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <cstddef>

namespace chemish::rhi::vulkan {

class Device;

class Buffer {
public:
  Buffer(Device &device, size_t size, VkBufferUsageFlags usage,
         const void *initialData = nullptr);
  ~Buffer();

  Buffer(const Buffer &) = delete;
  Buffer &operator=(const Buffer &) = delete;

  Buffer(Buffer &&other) noexcept;
  Buffer &operator=(Buffer &&) = delete;

  const VkBuffer &getHandle() const { return handle; }
  size_t getSize() const { return size; }

  void write(const void *data, size_t size);

private:
  Device *device = nullptr;
  VkBuffer handle = VK_NULL_HANDLE;
  VmaAllocation allocation = VK_NULL_HANDLE;
  size_t size = 0;
};

} // namespace chemish::rhi::vulkan
