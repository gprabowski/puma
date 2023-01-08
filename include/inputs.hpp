#pragma once

#include <bitset>

#include <math.hpp>
#include <optional>

namespace pusn {

struct camera_meta {
  float yaw{-90.0f};
  float pitch{0.0f};
  float roll{0.0f};
  glm::vec3 pos{10.0f, 20.0f, 50.0f};
  glm::vec3 front{0.0f, 0.0f, -1.0f};
  glm::vec3 up{0.0f, 1.0f, 0.0f};
};

struct render_meta {
  float clip_near = 0.1f;
  float clip_far = 10000.f;
  float fov_y = 90.f;
};

template <int N> struct input_bitsets {
  std::bitset<N> pressed;
  std::bitset<N> just_pressed;
};

struct mouse_state : input_bitsets<4> {
  enum mouse_button : int { left = 0, right = 1, middle = 2, other = 3 };
  glm::vec2 last_pos;
  std::optional<math::vec2> reoriented;
};

struct keyboard_state : input_bitsets<1024> {};

// input state contains all information
// that the user is in control of, either
// via a GUI or direct keyboard manipulation
struct input_state {
  camera_meta camera;
  render_meta render_info;

  mouse_state mouse;
  keyboard_state keyboard;

  void reorient_camera(double xpos, double ypos);
  void handle_keyboard();
  void process_new_input();
};
} // namespace pusn
