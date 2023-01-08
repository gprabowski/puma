#include <inputs.hpp>

#include <glfw_impl.hpp>

namespace pusn {
void input_state::reorient_camera(double xpos, double ypos) {
  float xoffset = xpos - mouse.last_pos.x;
  float yoffset = mouse.last_pos.y -
                  ypos; // reversed since y-coordinates range from bottom to top
  mouse.last_pos = {xpos, ypos};

  const float sensitivity = 0.1f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  camera.yaw += xoffset;
  camera.pitch = std::clamp<float>(camera.pitch + yoffset, -89, 89);
  glm::vec3 direction;
  direction.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
  direction.y = sin(glm::radians(camera.pitch));
  direction.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
  camera.front = glm::normalize(direction);
}

void input_state::handle_keyboard() {
  static float delta_time = 0.0f;
  static float last_frame = 0.0f;

  float currentFrame = glfw_impl::get_ticks();
  delta_time = currentFrame - last_frame;
  last_frame = currentFrame;

  const float camera_speed = 300.f * delta_time; // adjust accordingly
  if (keyboard.pressed[glfw_impl::key_mappings::key_up]) {
    camera.pos += camera_speed * camera.up;
  }
  if (keyboard.pressed[glfw_impl::key_mappings::key_down]) {
    camera.pos -= camera_speed * camera.up;
  }
  if (keyboard.pressed[glfw_impl::key_mappings::key_forward]) {
    camera.pos += camera_speed * camera.front;
  }
  if (keyboard.pressed[glfw_impl::key_mappings::key_backward]) {
    camera.pos -= camera_speed * camera.front;
  }
  if (keyboard.pressed[glfw_impl::key_mappings::key_left]) {
    camera.pos -=
        glm::normalize(glm::cross(camera.front, camera.up)) * camera_speed;
  }
  if (keyboard.pressed[glfw_impl::key_mappings::key_right]) {
    camera.pos +=
        glm::normalize(glm::cross(camera.front, camera.up)) * camera_speed;
  }
}

void input_state::process_new_input() {
  // update camera position here
  if (mouse.reoriented.has_value()) {
    float xpos = mouse.reoriented.value().x;
    float ypos = mouse.reoriented.value().y;
    reorient_camera(xpos, ypos);
    mouse.reoriented.reset();
  }
  handle_keyboard();
}
} // namespace pusn
