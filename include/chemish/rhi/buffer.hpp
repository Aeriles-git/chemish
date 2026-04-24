#pragma once

#include <cstddef>

namespace chemish::rhi {

class Buffer {
public:
  virtual ~Buffer() = default;
  virtual size_t getSize() const = 0;
};

} // namespace chemish::rhi
