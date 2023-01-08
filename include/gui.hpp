#pragma once

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

#include <implot/implot.h>

#include <interpolator_scene.hpp>

#include <glfw_impl.hpp>
#include <inputs.hpp>

namespace pusn {

namespace gui {

namespace chosen_api = glfw_impl;

bool init(chosen_api::window_t &w);
void render(input_state &input, interpolator_scene &scene);
void start_frame();
void render_popups();
void render_main_menu(interpolator_scene &scene);
void end_frame();
void update_viewport_info(std::function<void(void)> process_input);

} // namespace gui

} // namespace pusn
