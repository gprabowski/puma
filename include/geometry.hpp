#pragma once

#include <math.hpp>

#include <vector>

namespace pusn {

struct pos_norm_col {
  math::vec3 pos;
  math::vec3 normal;
  math::vec3 color;
};

struct scene_object_info {
  math::vec3 position{0.f, 0.f, 0.f};
  math::vec3 rotation{0.f, 0.f, 0.f};
  math::vec3 scale{1.f, 1.f, 1.f};
};

struct api_agnostic_geometry {
  std::vector<pos_norm_col> vertices;
  std::vector<unsigned int> indices;

  math::vec4 color{1.f, 0.f, 0.f, 1.f};
};

} // namespace pusn
