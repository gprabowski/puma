#pragma once

#include <chrono>
#include <vector>

#include <glad/glad.h>

#include <geometry.hpp>
#include <glfw_impl.hpp>
#include <math.hpp>
#include <mock_data.hpp>

#include <atomic>

namespace pusn {

namespace internal {
// each of them needs to store:
//    * geometry
//    * API object reference

struct puma_state {
  float base_x{10.f}, base_y{10.f};
  float l1{15.f};
  float q2{10.f};
  float l3{5.f};
  float l4{5.f};

  float alpha_1{0};
  float alpha_2{0};
  float alpha_3{0};
  float alpha_4{0};
  float alpha_5{0};
};

inline float anorm(float a) { return std::fmod(a + 200 * 360, 360); }

inline float amix(float a, float b, float t) {
  if (std::abs(a - b) <= 180) {
    return glm::mix(a, b, t);
  } else {
    return anorm((b >= a) ? glm::mix(a, b - 360, t) : glm::mix(a, b + 360, t));
  }
}

inline float adist(float a, float b) {
  if (std::abs(a - b) <= 180) {
    return std::abs(a - b);
  } else {
    return (b >= a) ? std::abs(a - (b - 360)) : std::abs(a - (b + 360));
  }
}

inline float sq(float a) { return a * a; }

inline float state_dist(const puma_state &a, const puma_state &b) {
  return std::sqrt(
      sq(a.q2 - b.q2) + sq(adist(a.alpha_1, b.alpha_1)) +
      sq(adist(a.alpha_2, b.alpha_2)) + sq(adist(a.alpha_3, b.alpha_3)) +
      sq(adist(a.alpha_4, b.alpha_4)) + sq(adist(a.alpha_5, b.alpha_5)));
}

inline puma_state lerp(const puma_state &a, const puma_state &b, float t) {
  return puma_state{a.base_x,
                    a.base_y,
                    a.l1,
                    glm::mix(a.q2, b.q2, t),
                    a.l3,
                    a.l4,
                    amix(a.alpha_1, b.alpha_1, t),
                    amix(a.alpha_2, b.alpha_2, t),
                    amix(a.alpha_3, b.alpha_3, t),
                    amix(a.alpha_4, b.alpha_4, t),
                    amix(a.alpha_5, b.alpha_5, t)};
}

struct simulation_settings {
  float length{5.f};
  decltype(std::chrono::system_clock::now()) start_time;

  math::vec3 position_start{0.f, 0.f, 0.f};
  math::vec3 position_end{500.f, 0.f, 0.f};

  glm::quat quat_rotation_start{1.f, 0.f, 0.f, 0.f};
  glm::quat quat_rotation_end{1.f, 0.f, 0.f, 0.f};

  puma_state start_state;
  puma_state end_state;
};

struct light {
  scene_object_info placement{{200.f, 100.f, 200.f}, {}, {}};
  math::vec3 color{1.f, 1.f, 1.f};
};

struct scene_grid {
  api_agnostic_geometry geometry{{{math::vec3(-1.0, 0.0, -1.0), {}, {}},
                                  {math::vec3(1.0, 0.0, -1.0), {}, {}},
                                  {math::vec3(1.0, 0.0, 1.0), {}, {}},
                                  {math::vec3(-1.0, 0.0, 1.0), {}, {}}},
                                 {0, 1, 2, 2, 3, 0}};
  scene_object_info placement;
  glfw_impl::renderable api_renderable;
};

struct puma_geometry {
  api_agnostic_geometry base{{{math::vec3(-1.0, 0.0, -1.0), {}, {}},
                              {math::vec3(1.0, 0.0, -1.0), {}, {}},
                              {math::vec3(1.0, 0.0, 1.0), {}, {}},
                              {math::vec3(-1.0, 0.0, 1.0), {}, {}}},
                             {0, 1, 2, 2, 3, 0}};
  api_agnostic_geometry arm_1;
  api_agnostic_geometry joint_12;
  api_agnostic_geometry arm_2;
  api_agnostic_geometry joint_23;
  api_agnostic_geometry arm_3;
  api_agnostic_geometry arm_4;
  api_agnostic_geometry spike_x;
  api_agnostic_geometry spike_y;
  api_agnostic_geometry spike_z;

  void clear() {
    base.vertices.clear();
    base.indices.clear();

    arm_1.vertices.clear();
    arm_1.indices.clear();

    joint_12.vertices.clear();
    joint_12.indices.clear();

    arm_2.vertices.clear();
    arm_2.indices.clear();

    joint_23.vertices.clear();
    joint_23.indices.clear();

    arm_3.vertices.clear();
    arm_3.indices.clear();

    arm_4.vertices.clear();
    arm_4.indices.clear();

    spike_x.vertices.clear();
    spike_x.indices.clear();

    spike_y.vertices.clear();
    spike_y.indices.clear();

    spike_z.vertices.clear();
    spike_z.indices.clear();
  }
};

struct puma_renderable {
  glfw_impl::renderable base;
  glfw_impl::renderable arm_1;
  glfw_impl::renderable joint_12;
  glfw_impl::renderable arm_2;
  glfw_impl::renderable joint_23;
  glfw_impl::renderable arm_3;
  glfw_impl::renderable arm_4;
  glfw_impl::renderable spike_x;
  glfw_impl::renderable spike_y;
  glfw_impl::renderable spike_z;
};

struct model {
  std::optional<simulation_settings> current_settings;
  simulation_settings next_settings;

  puma_state left_puma{};
  puma_state right_puma{};

  puma_geometry geometry;
  puma_renderable renderable;

  inline void reset() {
    geometry.clear();

    geometry.base = {
        {{math::vec3(-1.0, 0.1, -1.0), {0.f, 1.f, 0.f}, {0.8f, 0.8f, 0.8f}},
         {math::vec3(1.0, 0.1, -1.0), {0.f, 1.f, 0.f}, {0.8f, 0.8f, 0.8f}},
         {math::vec3(1.0, 0.1, 1.0), {0.f, 1.f, 0.f}, {0.8f, 0.8f, 0.8f}},
         {math::vec3(-1.0, 0.1, 1.0), {0.f, 1.f, 0.f}, {0.8f, 0.8f, 0.8f}}},
        {0, 1, 2, 2, 3, 0}};

    auto rot_m = glm::toMat4(glm::quat({glm::pi<float>() / 2.f, 0.f, 0.f}));
    mock_data::build_vertices_helper(
        50, left_puma.l1, 1.f, geometry.arm_1.vertices, geometry.arm_1.indices,
        rot_m, {0.8f, 0.8f, 0.8f});

    rot_m = glm::toMat4(glm::quat({0.f, 0.f, 0.f}));
    mock_data::build_vertices_helper(50, 2.f, 1.f, geometry.joint_12.vertices,
                                     geometry.joint_12.indices, rot_m,
                                     {0.8f, 0.8f, 0.8f});

    rot_m = glm::toMat4(glm::quat({0.f, -glm::pi<float>() / 2.f, 0.f}));
    mock_data::build_vertices_helper(
        50, left_puma.q2, 1.f, geometry.arm_2.vertices, geometry.arm_2.indices,
        rot_m, {0.8f, 0.8f, 0.8f});

    // joint23
    rot_m = glm::toMat4(glm::quat({0.f, 0.f, 0.f}));
    mock_data::build_vertices_helper(50, 2.f, 1.f, geometry.joint_23.vertices,
                                     geometry.joint_23.indices, rot_m,
                                     {0.8f, 0.8f, 0.8f});

    // arm3
    rot_m = glm::toMat4(glm::quat({-glm::pi<float>() / 2.f, 0.f, 0.f}));
    mock_data::build_vertices_helper(
        50, left_puma.l3, 1.f, geometry.arm_3.vertices, geometry.arm_3.indices,
        rot_m, {0.8f, 0.8f, 0.8f});

    // arm4
    rot_m = glm::toMat4(glm::quat({0.f, -glm::pi<float>() / 2.f, 0.f}));
    mock_data::build_vertices_helper(
        50, left_puma.l4, 1.f, geometry.arm_4.vertices, geometry.arm_4.indices,
        rot_m, {0.8f, 0.8f, 0.8f});

    // spike x
    rot_m = glm::toMat4(glm::quat({0.f, -glm::pi<float>() / 2.f, 0.f}));
    mock_data::build_vertices_helper(50, 2.f, 0.2f, geometry.spike_x.vertices,
                                     geometry.spike_x.indices, rot_m,
                                     {1.0f, 0.0f, 0.0f});

    // spike y
    rot_m = glm::toMat4(glm::quat({0.f, 0.f, 0.f}));
    mock_data::build_vertices_helper(50, 2.f, 0.2f, geometry.spike_y.vertices,
                                     geometry.spike_y.indices, rot_m,
                                     {0.0f, 1.0f, 0.0f});

    // spike z
    rot_m = glm::toMat4(glm::quat({glm::pi<float>() / 2.f, 0.f, 0.f}));
    mock_data::build_vertices_helper(50, 2.f, 0.2f, geometry.spike_z.vertices,
                                     geometry.spike_z.indices, rot_m,
                                     {0.0f, 0.0f, 1.0f});

    glfw_impl::fill_renderable(geometry.base.vertices, geometry.base.indices,
                               renderable.base);
    glfw_impl::add_program_to_renderable("resources/model", renderable.base);

    glfw_impl::fill_renderable(geometry.arm_1.vertices, geometry.arm_1.indices,
                               renderable.arm_1);
    glfw_impl::add_program_to_renderable("resources/model", renderable.arm_1);

    glfw_impl::fill_renderable(geometry.joint_12.vertices,
                               geometry.joint_12.indices, renderable.joint_12);
    glfw_impl::add_program_to_renderable("resources/model",
                                         renderable.joint_12);

    glfw_impl::fill_renderable(geometry.arm_2.vertices, geometry.arm_2.indices,
                               renderable.arm_2);
    glfw_impl::add_program_to_renderable("resources/model", renderable.arm_2);

    glfw_impl::fill_renderable(geometry.joint_23.vertices,
                               geometry.joint_23.indices, renderable.joint_23);
    glfw_impl::add_program_to_renderable("resources/model",
                                         renderable.joint_23);

    glfw_impl::fill_renderable(geometry.arm_3.vertices, geometry.arm_3.indices,
                               renderable.arm_3);
    glfw_impl::add_program_to_renderable("resources/model", renderable.arm_3);

    glfw_impl::fill_renderable(geometry.arm_4.vertices, geometry.arm_4.indices,
                               renderable.arm_4);
    glfw_impl::add_program_to_renderable("resources/model", renderable.arm_4);

    // spike x
    glfw_impl::fill_renderable(geometry.spike_x.vertices,
                               geometry.spike_x.indices, renderable.spike_x);
    glfw_impl::add_program_to_renderable("resources/model", renderable.spike_x);

    // spike x
    glfw_impl::fill_renderable(geometry.spike_y.vertices,
                               geometry.spike_y.indices, renderable.spike_y);
    glfw_impl::add_program_to_renderable("resources/model", renderable.spike_y);

    // spike x
    glfw_impl::fill_renderable(geometry.spike_z.vertices,
                               geometry.spike_z.indices, renderable.spike_z);
    glfw_impl::add_program_to_renderable("resources/model", renderable.spike_z);
  }
};

} // namespace internal

struct interpolator_scene {
  internal::simulation_settings settings;
  internal::model model;
  internal::scene_grid grid;
  internal::light light;

  bool init();
  void render(input_state &input, bool left = true);
  void set_light_uniforms(input_state &input, glfw_impl::renderable &r);
};

} // namespace pusn
