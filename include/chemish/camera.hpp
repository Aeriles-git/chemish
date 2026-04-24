#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace chemish {

struct Camera {
  glm::vec3 position{0.0f, 0.0f, 2.0f};
  glm::vec3 target{0.0f, 0.0f, 0.0f};
  glm::vec3 up{0.0f, 1.0f, 0.0f};
  float fovY = glm::radians(60.0f);
  float aspect = 16.0f / 9.0f;
  float zNear = 0.1f;
  float zFar = 100.0f;

  glm::mat4 view() const { return glm::lookAt(position, target, up); }

  glm::mat4 projection() const {
    glm::mat4 p = glm::perspective(fovY, aspect, zNear, zFar);
    p[1][1] *= -1.0f; // Vulkan has inverted Y vs OpenGL
    return p;
  }
};

// The exact struct the shader reads. Layout matters — std140/std430 rules.
struct CameraUniform {
  glm::mat4 viewProj;
};

} // namespace chemish
