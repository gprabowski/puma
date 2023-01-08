#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtx/quaternion.hpp>

namespace math {
using vec4 = glm::vec4;
using vec3 = glm::vec3;
using vec2 = glm::vec2;

using mat4 = glm::mat4;
using mat3 = glm::mat3;

struct char3 {
  char x{0}, y{0}, z{0};
};

struct char4 {
  char x{0}, y{0}, z{0}, w{0};
};

struct int2 {
  int x{0}, y{0};
};

template <typename MathType> inline auto get_value_ptr(const MathType &value) {
  return glm::value_ptr(value);
}

template <typename DataType>
constexpr inline auto deg_to_rad(const DataType &deg) {
  return glm::radians(deg);
}

inline mat4 get_projection_matrix(float fov_y_radians, float buffer_w,
                                  float buffer_h, float clip_near,
                                  float clip_far) {
  return glm::perspective(fov_y_radians, buffer_w / buffer_h, clip_near,
                          clip_far);
}

inline mat4 get_view_matrix(const vec3 &camera_pos, const vec3 &camera_front,
                            const vec3 &camera_up) {
  return glm::lookAt(camera_pos, camera_front, camera_up);
}

inline mat4 get_model_matrix(const vec3 &position, const vec3 &scale,
                             const vec3 &rotation_radians) {
  const auto trans_m = glm::translate(glm::mat4(1.0f), position);
  const auto scale_m = glm::scale(glm::mat4(1.0f), scale);
  const auto rot_m = glm::toMat4(glm::quat(rotation_radians));

  return trans_m * scale_m * rot_m;
}

inline glm::quat slerp(glm::quat const &x, glm::quat const &y, float t) {
  glm::quat z = y;

  float cosTheta = glm::dot(x, y);

  if (cosTheta < 0.f) {
    z = -y;
    cosTheta = -cosTheta;
  }

  if (cosTheta > 1.f - std::numeric_limits<float>::epsilon()) {
    return glm::normalize(
        glm::quat(glm::mix(x.w, z.w, t), glm::mix(x.x, z.x, t),
                  glm::mix(x.y, z.y, t), glm::mix(x.z, z.z, t)));
  } else {
    float angle = acos(cosTheta);
    return glm::normalize((sinf((1.f - t) * angle) * x + sinf(t * angle) * z) /
                          sinf(angle));
  }
}

inline glm::quat lerp(glm::quat const &x, glm::quat const &y, float t) {
  glm::quat z = y;

  float cosTheta = glm::dot(x, y);

  if (cosTheta < 0.f) {
    z = -y;
    cosTheta = -cosTheta;
  }

  return glm::normalize(glm::quat(glm::mix(x.w, z.w, t), glm::mix(x.x, z.x, t),
                                  glm::mix(x.y, z.y, t),
                                  glm::mix(x.z, z.z, t)));
}

} // namespace math
