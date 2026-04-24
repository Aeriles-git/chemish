#include <chemish/camera.hpp>
#include <chemish/mesh.hpp>
#include <chemish/renderer.hpp>
#include <chemish/vertex.hpp>

#include <SDL3/SDL.h>

#include <vector>

int main() {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window =
      SDL_CreateWindow("chemish", 1280, 720, SDL_WINDOW_VULKAN);

  {
    chemish::Renderer renderer{window};

    std::vector<chemish::Vertex> vertices = {
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    };
    chemish::MeshHandle triangle = renderer.createMesh(vertices);
    chemish::Camera camera;

    bool running = true;
    while (running) {
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT)
          running = false;
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
          running = false;
      }
      renderer.updateCamera(camera);
      renderer.drawFrame(triangle);
    }
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
