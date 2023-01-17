#include <gui.hpp>

#include <chrono>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

#include <implot/implot.h>

#include <ImGuiFileDialog.h>

#include <inverse_kinematics.hpp>

namespace pusn {
namespace gui {

struct gui_info {
  static std::string file_error_message;
};

std::string gui_info::file_error_message{""};

// utility structure for realtime plot
struct ScrollingBuffer {
  int MaxSize;
  int Offset;
  ImVector<ImVec2> Data;
  ScrollingBuffer(int max_size = 2000) {
    MaxSize = max_size;
    Offset = 0;
    Data.reserve(MaxSize);
  }
  void AddPoint(float x, float y) {
    if (Data.size() < MaxSize)
      Data.push_back(ImVec2(x, y));
    else {
      Data[Offset] = ImVec2(x, y);
      Offset = (Offset + 1) % MaxSize;
    }
  }
  void Erase() {
    if (Data.size() > 0) {
      Data.shrink(0);
      Offset = 0;
    }
  }
};

// utility structure for realtime plot
struct RollingBuffer {
  float Span;
  ImVector<ImVec2> Data;
  RollingBuffer() {
    Span = 10.0f;
    Data.reserve(2000);
  }
  void AddPoint(float x, float y) {
    float xmod = fmodf(x, Span);
    if (!Data.empty() && xmod < Data.back().x)
      Data.shrink(0);
    Data.push_back(ImVec2(xmod, y));
  }
};

void ShowDemo_RealtimePlots() {
  static ScrollingBuffer sdata1;
  static RollingBuffer rdata1;
  static float t = 0;
  t += ImGui::GetIO().DeltaTime;
  sdata1.AddPoint(t, chosen_api::last_frame_info::last_frame_time);
  rdata1.AddPoint(t, ImGui::GetIO().Framerate);

  static float history = 10.0f;
  ImGui::SliderFloat("History", &history, 1, 30, "%.1f s");
  rdata1.Span = history;

  static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;

  if (ImPlot::BeginPlot("##Scrolling", ImVec2(-1, 150))) {
    ImPlot::SetupAxes(NULL, NULL, flags, flags);
    ImPlot::SetupAxisLimits(ImAxis_X1, t - history, t, ImGuiCond_Always);
    ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 16.f);
    ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);
    ImPlot::PlotShaded("Frame time in ms", &sdata1.Data[0].x, &sdata1.Data[0].y,
                       sdata1.Data.size(), -INFINITY, 0, sdata1.Offset,
                       2 * sizeof(float));
    ImPlot::EndPlot();
  }
  if (ImPlot::BeginPlot("##Rolling", ImVec2(-1, 150))) {
    ImPlot::SetupAxes(NULL, NULL, flags, flags);
    ImPlot::SetupAxisLimits(ImAxis_X1, 0, history, ImGuiCond_Always);
    ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100);
    ImPlot::PlotLine("FPS", &rdata1.Data[0].x, &rdata1.Data[0].y,
                     rdata1.Data.size(), 0, 0, 2 * sizeof(float));
    ImPlot::EndPlot();
  }
}

// color theme copied from thecherno/hazel
void set_dark_theme() {
  auto &colors = ImGui::GetStyle().Colors;
  colors[ImGuiCol_WindowBg] = ImVec4{0.1f, 0.105f, 0.11f, 1.0f};

  // Headers
  colors[ImGuiCol_Header] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
  colors[ImGuiCol_HeaderHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
  colors[ImGuiCol_HeaderActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

  // Buttons
  colors[ImGuiCol_Button] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
  colors[ImGuiCol_ButtonHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
  colors[ImGuiCol_ButtonActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

  // Frame BG
  colors[ImGuiCol_FrameBg] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
  colors[ImGuiCol_FrameBgHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
  colors[ImGuiCol_FrameBgActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

  // Tabs
  colors[ImGuiCol_Tab] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
  colors[ImGuiCol_TabHovered] = ImVec4{0.38f, 0.3805f, 0.381f, 1.0f};
  colors[ImGuiCol_TabActive] = ImVec4{0.28f, 0.2805f, 0.281f, 1.0f};
  colors[ImGuiCol_TabUnfocused] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
  colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};

  // Title
  colors[ImGuiCol_TitleBg] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
  colors[ImGuiCol_TitleBgActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
}

bool init(chosen_api::window_t &w) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  // io.ConfigViewportsNoAutoMerge = true;
  // io.ConfigViewportsNoTaskBarIcon = true;

  ImGui::StyleColorsDark();

  // when viewports are enables we tweak WindowRounding/WIndowBg so platform
  // windows can look identical to regular ones
  ImGuiStyle &style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(w.get(), true);
  ImGui_ImplOpenGL3_Init("#version 460");

  // fonts
  io.FontDefault = io.Fonts->AddFontFromFileTTF(
      //"fonts/opensans/static/OpenSans/OpenSans-Regular.ttf",
      "resources/fonts/jbmono/fonts/ttf/JetBrainsMono-Regular.ttf", 18.0f);
  set_dark_theme();

  return true;
}

void start_frame() {
  static bool show_demo = false;
  ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockspace_flags);

  if (show_demo) {
    ImGui::ShowDemoWindow(&show_demo);
    ImPlot::ShowDemoWindow();
  }
}

void update_viewport_info(std::function<void(void)> process_input) {
  // update viewport static info
  ImGui::Begin("Position Interpolation");

  auto min = ImGui::GetWindowContentRegionMin();
  auto max = ImGui::GetWindowContentRegionMax();

  chosen_api::last_frame_info::left_viewport_area = {max.x - min.x,
                                                     max.y - min.y};

  auto tmp = ImGui::GetWindowPos();
  chosen_api::last_frame_info::left_viewport_pos = {tmp.x, tmp.y};
  chosen_api::last_frame_info::left_viewport_pos = {tmp.x + min.x,
                                                    tmp.y + min.y};

  ImVec2 cp = {chosen_api::last_frame_info::left_viewport_pos.x,
               chosen_api::last_frame_info::left_viewport_pos.y};

  ImVec2 ca = {chosen_api::last_frame_info::left_viewport_area.x,
               chosen_api::last_frame_info::left_viewport_area.y};

  if (ImGui::IsMouseHoveringRect(cp, {cp.x + ca.x, cp.y + ca.y})) {
    process_input();
  }

  ImGui::End();

  ImGui::Begin("Solution Interpolation");

  min = ImGui::GetWindowContentRegionMin();
  max = ImGui::GetWindowContentRegionMax();

  chosen_api::last_frame_info::right_viewport_area = {max.x - min.x,
                                                      max.y - min.y};

  tmp = ImGui::GetWindowPos();
  chosen_api::last_frame_info::right_viewport_pos = {tmp.x, tmp.y};
  chosen_api::last_frame_info::right_viewport_pos = {tmp.x + min.x,
                                                     tmp.y + min.y};

  cp = {chosen_api::last_frame_info::right_viewport_pos.x,
        chosen_api::last_frame_info::right_viewport_pos.y};

  ca = {chosen_api::last_frame_info::right_viewport_area.x,
        chosen_api::last_frame_info::right_viewport_area.y};

  if (ImGui::IsMouseHoveringRect(cp, {cp.x + ca.x, cp.y + ca.y})) {
    process_input();
  }

  ImGui::End();
}

void render_performance_window() {
  ImGui::Begin("Frame Statistics");
  ShowDemo_RealtimePlots();
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
              1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::Text("Last CPU frame %.3lf ms",
              glfw_impl::last_frame_info::last_frame_time);
  ImGui::End();
}

void render_light_gui(internal::light &light) {
  ImGui::Begin("Light Settings");
  ImGui::DragFloat3("Position", glm::value_ptr(light.placement.position),
                    -10000.f, 10000.f);
  ImGui::ColorEdit3("Color", glm::value_ptr(light.color));
  ImGui::End();
}

void render_simulation_gui(internal::model &model) {
  ImGui::Begin("Left Puma Debug");
  ImGui::DragFloat("Length", &model.next_settings.length, 1.f, 20.f);

  ImGui::SliderFloat("Alpha1", &model.left_puma.alpha_1, 0.f, 360.f);
  ImGui::SliderFloat("Q2", &model.left_puma.q2, 0.f, 360.f);
  ImGui::SliderFloat("Alpha2", &model.left_puma.alpha_2, 0.f, 360.f);
  ImGui::SliderFloat("Alpha3", &model.left_puma.alpha_3, 0.f, 360.f);
  ImGui::SliderFloat("Alpha4", &model.left_puma.alpha_4, 0.f, 360.f);
  ImGui::SliderFloat("Alpha5", &model.left_puma.alpha_5, 0.f, 360.f);
  ImGui::End();

  ImGui::Begin("Simulation Settings");
  ImGui::DragFloat("Length", &model.next_settings.length, 1.f, 20.f);

  static glm::vec3 start_angle{};
  static glm::vec3 end_angle{};

  bool changed = false;
  changed = changed || ImGui::SliderFloat3(
                           "Start Position",
                           glm::value_ptr(model.next_settings.position_start),
                           -30.f, 30.f);
  if (ImGui::SliderFloat3("Start Orientation", glm::value_ptr(start_angle), 0.f,
                          360.f)) {
    changed = true;
    model.next_settings.quat_rotation_start =
        glm::quat(glm::radians(start_angle));
  }

  changed = changed ||
            ImGui::SliderFloat3(
                "End Position",
                glm::value_ptr(model.next_settings.position_end), -30.f, 30.f);

  if (ImGui::SliderFloat3("End Orientation", glm::value_ptr(end_angle), 0.f,
                          360.f)) {
    changed = true;
    model.next_settings.quat_rotation_end = glm::quat(glm::radians(end_angle));
  }

  auto cpos = get_actuator_pos(model.left_puma);
  ImGui::Text("Left actuator: %f, %f, %f", cpos.x, cpos.y, cpos.z);

  auto rcpos = get_actuator_pos(model.right_puma);
  ImGui::Text("Right actuator: %f, %f, %f", rcpos.x, rcpos.y, rcpos.z);

  if (ImGui::Button("Run")) {

    model.current_settings = model.next_settings;
    model.current_settings.value().start_time =
        std::chrono::system_clock::now();

    auto solutions_start =
        solve_task(model, {model.next_settings.position_start,
                           model.next_settings.quat_rotation_start});
    auto solutions_end =
        solve_task(model, {model.next_settings.position_end,
                           model.next_settings.quat_rotation_end});

    model.current_settings.value().start_state = solutions_start[0];
    model.current_settings.value().end_state = solutions_end[0];
  }

  ImGui::End();
}

void render_converter() {
  static glm::vec3 euler{0.f, 0.f, 0.f};
  static glm::quat quat{1.f, 0.f, 0.f, 0.f};

  ImGui::Begin("Rotation Converter");

  if (ImGui::DragFloat3("Euler", glm::value_ptr(euler), -2 * glm::pi<float>(),
                        2 * glm::pi<float>())) {
    quat = glm::quat(euler);
  }

  if (ImGui::DragFloat4("Quaternion", glm::value_ptr(quat),
                        -2 * glm::pi<float>(), 2 * glm::pi<float>())) {
    quat = glm::normalize(quat);
    euler = eulerAngles(quat);
  }

  ImGui::End();
}

void render(input_state &input, interpolator_scene &scene) {
  render_performance_window();
  render_light_gui(scene.light);
  render_simulation_gui(scene.model);
  render_converter();
  render_popups();
}

void end_frame() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  // update and render additional platform windows
  // (platform functions may change the current opengl context so we
  // save/restore it to make it easier to paste this code elsewhere. For
  // this specific demo appp we could also call
  // glfwMakeCOntextCurrent(window) directly)
  ImGuiIO &io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    GLFWwindow *backup_current_context = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(backup_current_context);
  }
}

void render_popups() {
  // Always center this window when appearing
  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

  if (ImGui::BeginPopupModal("File Corrupted", NULL,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("The file you have pointed to is corrupted or wrongly "
                "formatted!\n\n");
    ImGui::Text("%s\n", gui_info::file_error_message.c_str());
    ImGui::Separator();

    if (ImGui::Button("OK", ImVec2(120, 0))) {
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}

} // namespace gui
} // namespace pusn
