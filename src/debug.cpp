#include <chemish/debug.hpp>

#include <cstdio>

namespace chemish {

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT /*types*/,
    const VkDebugUtilsMessengerCallbackDataEXT *data, void * /*user*/) {
  const char *sev =
      (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)     ? "ERROR"
      : (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) ? "WARN"
      : (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)    ? "INFO"
                                                                  : "VERBOSE";
  std::fprintf(stderr, "[VK %s] %s\n", sev, data->pMessage);
  return VK_FALSE;
}

void fillDebugMessengerInfo(VkDebugUtilsMessengerCreateInfoEXT &info) {
  info = {};
  info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  info.pfnUserCallback = debugCallback;
}

VkDebugUtilsMessengerEXT createDebugMessenger(VkInstance instance) {
  auto fn = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");
  if (!fn)
    return VK_NULL_HANDLE;

  VkDebugUtilsMessengerCreateInfoEXT info;
  fillDebugMessengerInfo(info);

  VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
  fn(instance, &info, nullptr, &messenger);
  return messenger;
}

void destroyDebugMessenger(VkInstance instance,
                           VkDebugUtilsMessengerEXT messenger) {
  if (messenger == VK_NULL_HANDLE)
    return;
  auto fn = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");
  if (fn)
    fn(instance, messenger, nullptr);
}

} // namespace chemish
