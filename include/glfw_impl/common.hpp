#pragma once

#include <memory>
#include <optional>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <math.hpp>

namespace pusn {
namespace glfw_impl {

using window_t = std::shared_ptr<GLFWwindow>;

struct texture_t {
  int width, height;
  std::optional<GLuint> index;

  inline GLuint value() { return index.value(); }
  inline bool has_value() { return index.has_value(); }
};

struct last_frame_info {
  static unsigned int width;
  static unsigned int height;

  static math::vec2 left_viewport_area;
  static math::vec2 left_viewport_pos;

  static math::vec2 right_viewport_area;
  static math::vec2 right_viewport_pos;

  static float last_frame_time;
  static uint64_t begin_time;
};

struct key_mappings {
  static constexpr int key_left = GLFW_KEY_A;
  static constexpr int key_right = GLFW_KEY_D;
  static constexpr int key_down = GLFW_KEY_Q;
  static constexpr int key_up = GLFW_KEY_E;
  static constexpr int key_forward = GLFW_KEY_W;
  static constexpr int key_backward = GLFW_KEY_S;
};

enum class render_mode { triangles, patches, line_strip };

struct renderable {
  std::optional<GLuint> vao;
  std::optional<GLuint> ebo;
  std::optional<GLuint> vbo;

  std::optional<GLuint> program;
};
} // namespace glfw_impl
} // namespace pusn
