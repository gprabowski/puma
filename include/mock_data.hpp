#pragma once

#include <math.hpp>
#include <vector>

#include <geometry.hpp>

namespace mock_data {

inline std::vector<float> getUnitCircleVertices(int SectorCount) {
  const float PI = 3.1415926f;
  float sectorStep = 2 * PI / SectorCount;
  float sectorAngle; // radian

  std::vector<float> unitCircleVertices;
  for (int i = 0; i <= SectorCount; ++i) {
    sectorAngle = i * sectorStep;
    unitCircleVertices.push_back(cos(sectorAngle)); // x
    unitCircleVertices.push_back(sin(sectorAngle)); // y
    unitCircleVertices.push_back(0);                // z
  }
  return unitCircleVertices;
}

inline void build_vertices_helper(int SectorCount, float Height, float Radius,
                                  std::vector<pusn::pos_norm_col> &vertices,
                                  std::vector<unsigned int> &indices,
                                  const math::mat4 &transform,
                                  math::vec3 default_color = {1.f, 0.f, 0.f}) {
  const auto initial_idx = vertices.size();
  // clear memory of prev arrays
  std::vector<float> normals;
  std::vector<float> texCoords;

  // get unit circle vectors on XY-plane
  std::vector<float> unitVertices = getUnitCircleVertices(SectorCount);

  auto transformer = [=](const pusn::pos_norm_col &in) -> pusn::pos_norm_col {
    return {glm::vec3(transform * glm::vec4{in.pos, 1.f}),
            glm::mat3(glm::inverse(glm::transpose(transform))) * in.normal,
            in.color};
  };

  // put side vertices to arrays
  for (int i = 0; i < 2; ++i) {
    float h = -Height / 2.0f + i * Height; // z value; -h/2 to h/2

    for (int j = 0, k = 0; j <= SectorCount; ++j, k += 3) {
      float ux = unitVertices[k];
      float uy = unitVertices[k + 1];
      float uz = unitVertices[k + 2];
      // position vector
      vertices.push_back(
          transformer({{ux * Radius, uy * Radius, h - 0.5 * Height},
                       {ux, uy, uz},
                       default_color}));
    }
  }

  int baseCenterIndex = (int)vertices.size();
  int topCenterIndex =
      baseCenterIndex + SectorCount + 1; // include center vertex

  // put base and top vertices to arrays
  for (int i = 0; i < 2; ++i) {
    float h = -Height / 2.0f + i * Height; // z value; -h/2 to h/2
    float nz = -1 + i * 2;                 // z value of normal; -1 to 1

    // center point
    vertices.push_back(
        transformer({{0, 0, h - 0.5f * Height}, {0, 0, nz}, default_color}));

    for (int j = 0, k = 0; j < SectorCount; ++j, k += 3) {
      float ux = unitVertices[k];
      float uy = unitVertices[k + 1];
      // position vector
      vertices.push_back(
          transformer({{ux * Radius, uy * Radius, h - 0.5f * Height},
                       {0, 0, nz},
                       default_color}));
    }

    int k1 = 0;
    int k2 = SectorCount + 1;

    for (int i = 0; i < SectorCount; ++i, ++k1, ++k2) {
      indices.push_back(k1 + initial_idx);
      indices.push_back(k1 + 1 + initial_idx);
      indices.push_back(k2 + initial_idx);

      indices.push_back(k2 + initial_idx);
      indices.push_back(k1 + 1 + initial_idx);
      indices.push_back(k2 + 1 + initial_idx);
    }

    for (int i = 0, k = baseCenterIndex + 1; i < SectorCount; ++i, ++k) {
      if (i < SectorCount - 1) {
        indices.push_back(baseCenterIndex + initial_idx);
        indices.push_back(k + 1 + initial_idx);
        indices.push_back(k + initial_idx);
      } else // last triangle
      {
        indices.push_back(baseCenterIndex + initial_idx);
        indices.push_back(baseCenterIndex + 1 + initial_idx);
        indices.push_back(k + initial_idx);
      }
    }

    for (int i = 0, k = topCenterIndex + 1; i < SectorCount; ++i, ++k) {
      if (i < SectorCount - 1) {
        indices.push_back(topCenterIndex + initial_idx);
        indices.push_back(k + initial_idx);
        indices.push_back(k + 1 + initial_idx);
      } else // last triangle
      {
        indices.push_back(topCenterIndex + initial_idx);
        indices.push_back(k + initial_idx);
        indices.push_back(topCenterIndex + 1 + initial_idx);
      }
    }
  }
}

} // namespace mock_data
