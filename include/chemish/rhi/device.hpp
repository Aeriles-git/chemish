#pragma once

#include <chemish/rhi/handle.hpp>

#include <cstddef>
#include <memory>
#include <string>

namespace chemish::rhi {

// Forward declarations for resources the device creates.
class Buffer;
class Shader;
class Pipeline;

// Abstract device interface. Backends (Vulkan, etc.) implement this.
class Device {
public:
  virtual ~Device() = default;

  // Resources are created through the device and returned as owning pointers.
  virtual std::unique_ptr<Buffer>
  createVertexBuffer(size_t size, const void *initialData) = 0;
  virtual std::unique_ptr<Shader> createShader(const std::string &spvPath) = 0;

  // Pipeline creation will be more complex — added later.
};

} // namespace chemish::rhi
