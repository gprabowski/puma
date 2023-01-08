#pragma once

#include <chrono>
#include <vector>

#include <glad/glad.h>

#include <geometry.hpp>
#include <glfw_impl.hpp>
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
  float alpha_2{glm::pi<float>() / 2.f};
  float alpha_3{-glm::pi<float>() / 2.f};
  float alpha_4{0};
  float alpha_5{0};
};

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
