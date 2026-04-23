#pragma once

#include <chemish/rhi/handle.hpp>

#include <cassert>
#include <utility>
#include <vector>

namespace chemish::rhi {

// Generational pool: stores resources of type T, hands out Handle<Tag>s that
// can be validated against stale reuse.
//
// - insert(T) returns a Handle<Tag>.
// - get(Handle) returns T* or nullptr if the handle is stale.
// - remove(Handle) destroys the resource and bumps the slot's generation.
template <typename T, typename Tag> class Pool {
public:
  Handle<Tag> insert(T &&resource) {
    uint32_t index;
    if (!freeList.empty()) {
      index = freeList.back();
      freeList.pop_back();
      slots[index].resource = std::move(resource);
    } else {
      index = (uint32_t)slots.size();
      slots.push_back({std::move(resource), 1});
    }
    return Handle<Tag>{.index = index, .generation = slots[index].generation};
  }

  T *get(Handle<Tag> handle) {
    if (handle.index >= slots.size())
      return nullptr;
    if (slots[handle.index].generation != handle.generation)
      return nullptr;
    return &slots[handle.index].resource;
  }

  bool remove(Handle<Tag> handle) {
    T *resource = get(handle);
    if (!resource)
      return false;
    *resource = T{};                  // destroy (calls ~T)
    slots[handle.index].generation++; // invalidate old handles
    if (slots[handle.index].generation == 0) {
      slots[handle.index].generation = 1; // never go back to 0 (invalid)
    }
    freeList.push_back(handle.index);
    return true;
  }

  size_t size() const { return slots.size() - freeList.size(); }

private:
  struct Slot {
    T resource{};
    uint32_t generation = 1; // real slots start at 1; 0 means invalid handle
  };

  std::vector<Slot> slots;
  std::vector<uint32_t> freeList;
};

} // namespace chemish::rhi
