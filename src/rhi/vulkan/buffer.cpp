#include <chemish/rhi/vulkan/buffer.hpp>
#include <chemish/rhi/vulkan/device.hpp>

#include <cstring>

namespace chemish::rhi::vulkan {

Buffer::Buffer(Device &dev, size_t sz, VkBufferUsageFlags usage,
               const void *initialData)
    : device(&dev), size(sz) {
  VkBufferCreateInfo bufInfo{};
  bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufInfo.size = size;
  bufInfo.usage = usage;
  bufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo allocInfo{};
  allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
  allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                    VMA_ALLOCATION_CREATE_MAPPED_BIT;

  VmaAllocationInfo info{};
  vmaCreateBuffer(device->getAllocator(), &bufInfo, &allocInfo, &handle,
                  &allocation, &info);

  if (initialData) {
    std::memcpy(info.pMappedData, initialData, size);
  }
}

Buffer::Buffer(Buffer &&other) noexcept
    : device(other.device), handle(other.handle), allocation(other.allocation),
      size(other.size) {
  other.handle = VK_NULL_HANDLE;
  other.allocation = VK_NULL_HANDLE;
}

Buffer::~Buffer() {
  if (handle != VK_NULL_HANDLE) {
    vmaDestroyBuffer(device->getAllocator(), handle, allocation);
  }
}

} // namespace chemish::rhi::vulkan
