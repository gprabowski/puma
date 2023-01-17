#include <inverse_kinematics.hpp>

namespace pusn {

glm::vec3 get_actuator_pos(const internal::puma_state &state) {
  glm::vec4 res{state.l4, 0.f, 0.f, 1.f};
  glm::mat4 skinning_matrix = glm::mat4(1.f);
  auto mmat =
      math::get_model_matrix({0.f, 0.f, 0.f}, {5.f, 1.f, 5.f},
                             math::deg_to_rad(glm::vec3{0.f, 0.f, 0.f}));

  mmat = math::get_model_matrix({0.f, 0.f, 0.f}, {1.f, 1.f, 1.f},
                                math::deg_to_rad(glm::vec3{0.f, 0.f, 0.f}));

  mmat = math::get_model_matrix(
      {0.f, state.l1, 0.f}, {1.f, 1.f, 1.f},
      math::deg_to_rad(glm::vec3{0.f, state.alpha_1, 0.f}));
  skinning_matrix = mmat * skinning_matrix;

  // arm2
  mmat = math::get_model_matrix(
      {0.f, 0.f, 0.f}, {1.f, 1.f, 1.f},
      math::deg_to_rad(glm::vec3{0.f, 0.f, -state.alpha_2}));
  skinning_matrix = skinning_matrix * mmat;

  // joint23
  mmat = math::get_model_matrix({state.q2, 0.f, 0.f}, {1.f, 1.f, 1.f},
                                math::deg_to_rad(glm::vec3{0.f, 0.f, 0.f}));
  skinning_matrix = skinning_matrix * mmat;

  // arm3
  mmat = math::get_model_matrix(
      {0.f, 0.f, 0.f}, {1.f, 1.f, 1.f},
      math::deg_to_rad(glm::vec3{0.f, 0.f, -state.alpha_3}));
  skinning_matrix = skinning_matrix * mmat;

  // arm4
  mmat = math::get_model_matrix(
      {0.f, -state.l3, 0.f}, {1.f, 1.f, 1.f},
      math::deg_to_rad(glm::vec3{0.f, state.alpha_4, 0.f}));
  skinning_matrix = skinning_matrix * mmat;

  auto pret = skinning_matrix * res;
  pret.z *= -1;
  std::swap(pret.y, pret.z);
  return pret;
}

std::vector<internal::puma_state> solve_task(internal::model &model,
                                             const puma_pos &settings) {
  // solve the inverse config

  const static glm::vec3 def_x{1, 0, 0};
  const static glm::vec3 def_y{0, 1, 0};
  const static glm::vec3 def_z{0, 0, 1};
  // 1. solve for start position
  glm::vec3 start_x5 = glm::normalize(settings.rot * def_x);
  glm::vec3 start_y5 = glm::normalize(settings.rot * def_y);
  glm::vec3 start_z5 = glm::normalize(settings.rot * def_z);

  std::vector<internal::puma_state> solutions;
  solutions.reserve(8);
  solutions.push_back(model.left_puma);
  solutions[0].alpha_1 =
      atan((settings.pos.y - model.left_puma.l4 * start_x5.y) /
           (settings.pos.x - model.left_puma.l4 * start_x5.x));
  solutions.push_back(model.left_puma);
  solutions[1].alpha_1 = solutions[0].alpha_1 + glm::pi<float>();
  for (int i = 0; i < 2; ++i) {
    auto &sol = solutions[i];
    const auto c1 = std::cos(sol.alpha_1);
    const auto s1 = std::sin(sol.alpha_1);

    sol.alpha_4 = asin(c1 * start_x5.y - s1 * start_x5.x);
    auto copied = sol;
    copied.alpha_4 = sol.alpha_4 > 0 ? glm::pi<float>() - sol.alpha_4
                                     : -glm::pi<float>() - sol.alpha_4;
    solutions.push_back(copied);
  }

  for (int i = 0; i < 4; ++i) {
    auto &sol = solutions[i];
    auto &x5 = start_x5;
    auto &p5 = settings.pos;
    const auto c1 = std::cos(sol.alpha_1);
    const auto s1 = std::sin(sol.alpha_1);
    const auto c4 = std::cos(sol.alpha_4);
    const auto s4 = std::sin(sol.alpha_4);

    const auto c5 = (c1 * start_y5.y - s1 * start_y5.x) / c4;
    const auto s5 = (s1 * start_z5.x - c1 * start_z5.y) / c4;

    sol.alpha_5 = atan2(s5, c5);

    const auto nom = -(c1 * c4 * (p5.z - sol.l4 * x5.z - sol.l1) +
                       sol.l3 * (x5.x + s1 * s4));
    const auto den = c4 * (p5.x - sol.l4 * x5.x) - c1 * sol.l3 * x5.z;
    sol.alpha_2 = atan(nom / den);
    auto copied = sol;
    copied.alpha_2 = sol.alpha_2 + glm::pi<float>();
    solutions.push_back(copied);
  }

  for (int i = 0; i < 8; ++i) {
    auto &sol = solutions[i];
    auto &x5 = start_x5;
    auto &p5 = settings.pos;
    const auto c1 = std::cos(sol.alpha_1);
    const auto s1 = std::sin(sol.alpha_1);
    const auto c2 = std::cos(sol.alpha_2);
    const auto c4 = std::cos(sol.alpha_4);
    const auto s4 = std::sin(sol.alpha_4);

    sol.q2 =
        (c4 * (p5.x - sol.l4 * x5.x) - c1 * sol.l3 * x5.z) / (c1 * c2 * c4);

    const auto c23 = (x5.x + s1 * s4) / (c1 * c4);
    const auto s23 = -x5.z / c4;

    const auto a23 = atan2(s23, c23);
    sol.alpha_3 = a23 - sol.alpha_2;
  }

  auto anorm = [](float angle) {
    return fmod(angle + 2000 * glm::pi<float>(), 2 * glm::pi<float>());
  };

  for (auto &sol : solutions) {
    sol.alpha_1 = glm::degrees(anorm(sol.alpha_1));
    sol.alpha_2 = glm::degrees(anorm(sol.alpha_2));
    sol.alpha_3 = glm::degrees(anorm(sol.alpha_3));
    sol.alpha_4 = glm::degrees(anorm(sol.alpha_4));
    sol.alpha_5 = glm::degrees(anorm(sol.alpha_5));

    if (sol.q2 < 0) {
      sol.q2 *= -1;
      sol.alpha_2 += 180;
      if (sol.alpha_2 > 360) {
        sol.alpha_2 -= 360;
      }
      sol.alpha_3 += 180;
      if (sol.alpha_3 > 360) {
        sol.alpha_3 -= 360;
      }
    }
  }

  std::erase_if(solutions, [&](const auto &sol) {
    const auto lpos = get_actuator_pos(sol);
    const auto p5 = settings.pos;
    //LOGGER_INFO("Actuator: {0}, {1}, {2}", lpos.x, lpos.y, lpos.z);
    //LOGGER_INFO("Actuator: {0}, {1}, {2}", p5.x, p5.y, p5.z);

    const auto dist = glm::length(settings.pos - lpos);
    return (sol.q2 < 0) || (dist > 0.5f);
  });

  return solutions;
}
} // namespace pusn
