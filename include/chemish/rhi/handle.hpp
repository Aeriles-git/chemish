#pragma once

#include <cstdint>

namespace chemish::rhi {

template <typename Tag> struct Handle {
  uint32_t index = UINT32_MAX;
  uint32_t generation = 0;

  constexpr bool valid() const { return generation != 0; }
  constexpr bool operator==(const Handle &) const = default;
};

struct _BufferTag;
struct _TextureTag;
struct _ShaderTag;
struct _PipelineTag;
struct _SamplerTag;

using BufferHandle = Handle<_BufferTag>;
using TextureHandle = Handle<_TextureTag>;
using ShaderHandle = Handle<_ShaderTag>;
using PipelineHandle = Handle<_PipelineTag>;
using SamplerHandle = Handle<_SamplerTag>;

} // namespace chemish::rhi
