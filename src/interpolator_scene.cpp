#include <interpolator_scene.hpp>

#include <vector>

#include <math.hpp>

#include <mock_data.hpp>

namespace pusn {

void generate_milling_tool(api_agnostic_geometry &out) {}

bool interpolator_scene::init() {
  // Generate and add milling tool
  model.reset();

  // ADD GRID
  glfw_impl::fill_renderable(grid.geometry.vertices, grid.geometry.indices,
                             grid.api_renderable);
  glfw_impl::add_program_to_renderable("resources/grid", grid.api_renderable);

  return true;
}

void interpolator_scene::set_light_uniforms(input_state &input,
                                            glfw_impl::renderable &r) {
  // set light and camera uniforms
  glfw_impl::set_uniform("light_pos", r.program.value(),
                         light.placement.position);
  glfw_impl::set_uniform("light_color", r.program.value(), light.color);
  glfw_impl::set_uniform("cam_pos", r.program.value(), input.camera.pos);
}

void interpolator_scene::render(input_state &input, bool left) {

  // 1. get camera info
  glDepthFunc(GL_LESS);

  const auto view = math::get_view_matrix(
      input.camera.pos, input.camera.pos + input.camera.front, input.camera.up);

  const auto proj = math::get_projection_matrix(
      glm::radians(input.render_info.fov_y),
      left ? glfw_impl::last_frame_info::left_viewport_area.x
           : glfw_impl::last_frame_info::right_viewport_area.x,
      left ? glfw_impl::last_frame_info::left_viewport_area.y
           : glfw_impl::last_frame_info::right_viewport_area.y,
      input.render_info.clip_near, input.render_info.clip_far);

  // 2. render grid
  glDisable(GL_CULL_FACE);
  const auto model_grid_m =
      math::get_model_matrix(grid.placement.position, grid.placement.scale,
                             math::deg_to_rad(grid.placement.rotation));
  glfw_impl::use_program(grid.api_renderable.program.value());

  set_light_uniforms(input, grid.api_renderable);

  glfw_impl::set_uniform("model", grid.api_renderable.program.value(),
                         model_grid_m);
  glfw_impl::set_uniform("view", grid.api_renderable.program.value(), view);
  glfw_impl::set_uniform("proj", grid.api_renderable.program.value(), proj);
  glfw_impl::render(grid.api_renderable, grid.geometry);
  glEnable(GL_CULL_FACE);

  // 3. render the model
  const auto time = std::chrono::system_clock::now();
  if (model.current_settings.has_value()) {

    std::chrono::duration<float> elapsed_seconds =
        time - model.current_settings.value().start_time;
    const float progress =
        elapsed_seconds.count() / model.current_settings.value().length;

    if (progress > 1.0) {
      model.current_settings.reset();
    } else {
    }
  }

  auto render_element = [&](auto &renderable, auto &geometry, auto &mmat) {
    glfw_impl::use_program(renderable.program.value());
    set_light_uniforms(input, renderable);
    glfw_impl::set_uniform("model", renderable.program.value(), mmat);
    glfw_impl::set_uniform("view", renderable.program.value(), view);
    glfw_impl::set_uniform("proj", renderable.program.value(), proj);
    glfw_impl::render(renderable, geometry);
    glfw_impl::use_program(0);
  };

  if (left) {
    glm::mat4 skinning_matrix = glm::mat4(1.f);
    auto mmat =
        math::get_model_matrix({0.f, 0.f, 0.f}, {5.f, 1.f, 5.f},
                               math::deg_to_rad(glm::vec3{0.f, 0.f, 0.f}));
    glDisable(GL_CULL_FACE);
    render_element(model.renderable.base, model.geometry.base, mmat);
    glEnable(GL_CULL_FACE);

    mmat = math::get_model_matrix({0.f, 0.f, 0.f}, {1.f, 1.f, 1.f},
                                  math::deg_to_rad(glm::vec3{0.f, 0.f, 0.f}));
    render_element(model.renderable.arm_1, model.geometry.arm_1, mmat);

    // joint 12
    mmat = math::get_model_matrix(
        {0.f, model.left_puma.l1, 0.f}, {1.f, 1.f, 1.f},
        math::deg_to_rad(glm::vec3{0.f, model.left_puma.alpha_1, 0.f}));
    skinning_matrix = mmat * skinning_matrix;
    mmat = skinning_matrix * glm::translate(glm::mat4(1.f), {0.f, 0.f, 1.f});
    render_element(model.renderable.joint_12, model.geometry.joint_12, mmat);

    // arm2
    mmat = math::get_model_matrix(
        {0.f, 0.f, 0.f}, {1.f, 1.f, 1.f},
        math::deg_to_rad(glm::vec3{0.f, 0.f, model.left_puma.alpha_2}));
    skinning_matrix = skinning_matrix * mmat;
    mmat = skinning_matrix *
           glm::scale(glm::mat4(1.f), {model.left_puma.q2 / 10.f, 1.f, 1.f});
    render_element(model.renderable.arm_2, model.geometry.arm_2, mmat);

    // joint23
    mmat =
        math::get_model_matrix({model.left_puma.q2, 0.f, 0.f}, {1.f, 1.f, 1.f},
                               math::deg_to_rad(glm::vec3{0.f, 0.f, 0.f}));
    skinning_matrix = skinning_matrix * mmat;
    mmat = skinning_matrix * glm::translate(glm::mat4(1.f), {0.f, 0.f, 1.f});
    render_element(model.renderable.joint_23, model.geometry.joint_23, mmat);

    // arm3
    mmat = math::get_model_matrix(
        {0.f, 0.f, 0.f}, {1.f, 1.f, 1.f},
        math::deg_to_rad(glm::vec3{0.f, 0.f, model.left_puma.alpha_3}));
    skinning_matrix = skinning_matrix * mmat;
    render_element(model.renderable.arm_3, model.geometry.arm_3,
                   skinning_matrix);

    // arm4
    mmat = math::get_model_matrix(
        {0.f, -model.left_puma.l3, 0.f}, {1.f, 1.f, 1.f},
        math::deg_to_rad(glm::vec3{0.f, model.left_puma.alpha_4, 0.f}));
    skinning_matrix = skinning_matrix * mmat;
    render_element(model.renderable.arm_4, model.geometry.arm_4,
                   skinning_matrix);
  } else {
    glm::mat4 skinning_matrix = glm::mat4(1.f);
    auto mmat =
        math::get_model_matrix({0.f, 0.f, 0.f}, {5.f, 1.f, 5.f},
                               math::deg_to_rad(glm::vec3{0.f, 0.f, 0.f}));
    glDisable(GL_CULL_FACE);
    render_element(model.renderable.base, model.geometry.base, mmat);
    glEnable(GL_CULL_FACE);

    mmat = math::get_model_matrix({0.f, 0.f, 0.f}, {1.f, 1.f, 1.f},
                                  math::deg_to_rad(glm::vec3{0.f, 0.f, 0.f}));
    render_element(model.renderable.arm_1, model.geometry.arm_1, mmat);

    // joint 12
    mmat = math::get_model_matrix(
        {0.f, model.right_puma.l1, 0.f}, {1.f, 1.f, 1.f},
        math::deg_to_rad(glm::vec3{0.f, model.right_puma.alpha_1, 0.f}));
    skinning_matrix = mmat * skinning_matrix;
    mmat = skinning_matrix * glm::translate(glm::mat4(1.f), {0.f, 0.f, 1.f});
    render_element(model.renderable.joint_12, model.geometry.joint_12, mmat);

    // arm2
    mmat = math::get_model_matrix(
        {0.f, 0.f, 0.f}, {1.f, 1.f, 1.f},
        math::deg_to_rad(glm::vec3{0.f, 0.f, model.right_puma.alpha_2}));
    skinning_matrix = skinning_matrix * mmat;
    mmat = skinning_matrix *
           glm::scale(glm::mat4(1.f), {model.right_puma.q2 / 10.f, 1.f, 1.f});
    render_element(model.renderable.arm_2, model.geometry.arm_2, mmat);

    // joint23
    mmat =
        math::get_model_matrix({model.right_puma.q2, 0.f, 0.f}, {1.f, 1.f, 1.f},
                               math::deg_to_rad(glm::vec3{0.f, 0.f, 0.f}));
    skinning_matrix = skinning_matrix * mmat;
    mmat = skinning_matrix * glm::translate(glm::mat4(1.f), {0.f, 0.f, 1.f});
    render_element(model.renderable.joint_23, model.geometry.joint_23, mmat);

    // arm3
    mmat = math::get_model_matrix(
        {0.f, 0.f, 0.f}, {1.f, 1.f, 1.f},
        math::deg_to_rad(glm::vec3{0.f, 0.f, model.right_puma.alpha_3}));
    skinning_matrix = skinning_matrix * mmat;
    render_element(model.renderable.arm_3, model.geometry.arm_3,
                   skinning_matrix);

    // arm4
    mmat = math::get_model_matrix(
        {0.f, -model.right_puma.l3, 0.f}, {1.f, 1.f, 1.f},
        math::deg_to_rad(glm::vec3{0.f, model.right_puma.alpha_4, 0.f}));
    skinning_matrix = skinning_matrix * mmat;
    render_element(model.renderable.arm_4, model.geometry.arm_4,
                   skinning_matrix);
  }
}
} // namespace pusn
