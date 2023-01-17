#pragma once

#include <vector>

#include <math.hpp>

#include <interpolator_scene.hpp>

namespace pusn {

glm::vec3 get_actuator_pos(const internal::puma_state &state);

struct puma_pos {
  glm::vec3 pos;
  glm::quat rot;
};

std::vector<internal::puma_state> solve_task(internal::model &model,
                                             const puma_pos &settings);

} // namespace pusn
