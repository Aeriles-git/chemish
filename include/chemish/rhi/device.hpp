#pragma once

#include <chemish/rhi/handle.hpp>

namespace chemish::rhi {

// Abstract device. Backends (Vulkan, etc.) implement this.
// Resource methods will be added as we port each resource type.
class Device {
public:
  virtual ~Device() = default;
};

} // namespace chemish::rhi
