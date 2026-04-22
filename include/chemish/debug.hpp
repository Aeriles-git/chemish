#pragma once

#include <vulkan/vulkan.h>

namespace chemish {

// Populates a VkDebugUtilsMessengerCreateInfoEXT suitable for chaining into
// VkInstanceCreateInfo::pNext and for creating a persistent messenger.
void fillDebugMessengerInfo(VkDebugUtilsMessengerCreateInfoEXT &info);

// Creates a persistent debug messenger on the given instance.
// Returns VK_NULL_HANDLE if the extension isn't available.
VkDebugUtilsMessengerEXT createDebugMessenger(VkInstance instance);

// Destroys a messenger previously created with createDebugMessenger.
// Safe to call with VK_NULL_HANDLE.
void destroyDebugMessenger(VkInstance instance,
                           VkDebugUtilsMessengerEXT messenger);

// The validation layer name we require.
constexpr const char *kValidationLayerName = "VK_LAYER_KHRONOS_validation";

} // namespace chemish
