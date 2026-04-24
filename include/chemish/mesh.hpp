#pragma once

#include <cstdint>

namespace chemish {

struct MeshHandle {
  uint32_t id = 0;
  bool valid() const { return id != 0; }
};

} // namespace chemish
