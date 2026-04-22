// Step 1: the absolute minimum.
// - open an SDL3 window with Vulkan support
// - create a Vulkan instance with the validation layer
// - pick the first GPU and print its name
// - wait for the window to close
//
// No Volk, no VMA, no SDK helpers. Just the Vulkan loader talking to the driver.

#include <cstdio>
#include <cstdlib>
#include <vector>

#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

int main() {
    // ---- SDL + window ----
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("engine", 1280, 720, SDL_WINDOW_VULKAN);

    // ---- Instance ----
    VkApplicationInfo app{};
    app.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app.pApplicationName = "engine";
    app.apiVersion = VK_API_VERSION_1_3;

    // Ask SDL which instance extensions it needs for our platform (Wayland/X11).
    Uint32 extCount = 0;
    const char* const* sdlExts = SDL_Vulkan_GetInstanceExtensions(&extCount);
    std::vector<const char*> extensions(sdlExts, sdlExts + extCount);

    const char* layers[] = { "VK_LAYER_KHRONOS_validation" };

    VkInstanceCreateInfo ici{};
    ici.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    ici.pApplicationInfo = &app;
    ici.enabledLayerCount = 1;
    ici.ppEnabledLayerNames = layers;
    ici.enabledExtensionCount = (uint32_t)extensions.size();
    ici.ppEnabledExtensionNames = extensions.data();

    VkInstance instance = VK_NULL_HANDLE;
    if (vkCreateInstance(&ici, nullptr, &instance) != VK_SUCCESS) {
        std::fprintf(stderr, "vkCreateInstance failed\n");
        return 1;
    }

    // ---- Pick first GPU and print its name ----
    uint32_t gpuCount = 0;
    vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);
    std::vector<VkPhysicalDevice> gpus(gpuCount);
    vkEnumeratePhysicalDevices(instance, &gpuCount, gpus.data());

    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(gpus[0], &props);
    std::printf("GPU: %s\n", props.deviceName);

    // ---- Wait for quit ----
    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) running = false;
            if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_ESCAPE) running = false;
        }
    }

    // ---- Cleanup ----
    vkDestroyInstance(instance, nullptr);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
