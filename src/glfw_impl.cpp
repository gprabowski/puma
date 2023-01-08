#include <glfw_impl.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

#include <math.hpp>

#include <utils.hpp>

namespace pusn {

unsigned int glfw_impl::last_frame_info::width = 0;
unsigned int glfw_impl::last_frame_info::height = 0;

float glfw_impl::last_frame_info::last_frame_time = 0.f;
uint64_t glfw_impl::last_frame_info::begin_time = 0.f;

math::vec2 glfw_impl::last_frame_info::left_viewport_area = {};
math::vec2 glfw_impl::last_frame_info::left_viewport_pos = {};

math::vec2 glfw_impl::last_frame_info::right_viewport_area = {};
math::vec2 glfw_impl::last_frame_info::right_viewport_pos = {};

void glfw_impl::fill_renderable(std::vector<pos_norm_col> &vertices,
                                std::vector<unsigned int> &indices,
                                renderable &out) {
  if (!out.vbo.has_value()) {
    GLuint tmp;
    glCreateBuffers(1, &tmp);
    out.vbo = tmp;
  }
  // allocation or reallocation
  glNamedBufferData(out.vbo.value(), sizeof(pos_norm_col) * vertices.size(),
                    vertices.data(), GL_STATIC_DRAW);

  if (!out.ebo.has_value()) {
    GLuint tmp;
    glCreateBuffers(1, &tmp);
    out.ebo = tmp;
  }

  // allocation or reallocation
  glNamedBufferData(out.ebo.value(), sizeof(unsigned int) * indices.size(),
                    indices.data(), GL_STATIC_DRAW);

  if (!out.vao.has_value()) {
    GLuint tmp;
    glCreateVertexArrays(1, &tmp);
    out.vao = tmp;
  }

  // prepare array indexing among attributes
  glVertexArrayVertexBuffer(out.vao.value(), 0, out.vbo.value(), 0,
                            sizeof(vertices[0]));

  glVertexArrayElementBuffer(out.vao.value(), out.ebo.value());

  glEnableVertexArrayAttrib(out.vao.value(), 0);
  glEnableVertexArrayAttrib(out.vao.value(), 1);
  glEnableVertexArrayAttrib(out.vao.value(), 2);

  glVertexArrayAttribFormat(out.vao.value(), 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribFormat(out.vao.value(), 1, 3, GL_FLOAT, GL_FALSE,
                            3 * sizeof(float));
  glVertexArrayAttribFormat(out.vao.value(), 2, 3, GL_FLOAT, GL_FALSE,
                            6 * sizeof(float));

  glVertexArrayAttribBinding(out.vao.value(), 0, 0);
  glVertexArrayAttribBinding(out.vao.value(), 1, 0);
  glVertexArrayAttribBinding(out.vao.value(), 2, 0);
}

void glfw_impl::framebuffer_size_callback(GLFWwindow *window, int width,
                                          int height) {
  glViewport(0, 0, width, height);
  glfw_impl::last_frame_info::width = width;
  glfw_impl::last_frame_info::height = height;
}

void glfw_impl::teardown() { glfwTerminate(); }

void glfw_impl::error_callback(int error, const char *description) {
  LOGGER_ERROR("{0}: {1}", error, description);
}

void glfw_impl::die(const char *message) {
  const char *err;
  glfwGetError(&err);
  LOGGER_CRITICAL("{0} : {1} ", message, err);
  exit(-1);
}

void glfw_impl::window_destroyer(GLFWwindow *w) {
  LOGGER_INFO("Destroying GLFW window");
  glfwDestroyWindow(w);
}

glfw_impl::window_t glfw_impl::create_default_window(const int w, const int h,
                                                     const char *title) {
  // GLFW window hints
  glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
  glfwWindowHint(GLFW_DEPTH_BITS, 24);
  glfwWindowHint(GLFW_STENCIL_BITS, 8);
  glfwWindowHint(GLFW_SAMPLES, 8);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
  auto window =
      window_t(glfwCreateWindow(w, h, title, NULL, NULL), window_destroyer);

  if (!window) {
    die("Couldn't create a GLFW window");
  }

  glfwMakeContextCurrent(window.get());
  return window;
}

void glfw_impl::set_window_options(window_t &w, input_state *input) {
  glfwMakeContextCurrent(w.get());
  glfwSetWindowUserPointer(w.get(), reinterpret_cast<void *>(input));
  glfwSetFramebufferSizeCallback(w.get(), framebuffer_size_callback);
  // VSync ON
  glfwSwapInterval(1);
}

void glfw_impl::initialize_extensions() {
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    glfw_impl::die("[ERROR] Couldn't initialize GLAD");
  } else {
    LOGGER_INFO("GLAD Successfully initialized");
  }
}

void APIENTRY glfw_impl::openglCallbackFunction(GLenum source, GLenum type,
                                                GLuint id, GLenum severity,
                                                GLsizei length,
                                                const GLchar *message,
                                                const void *userParam) {
  (void)source;
  (void)type;
  (void)id;
  if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
    return;
  (void)length;
  (void)userParam;
  LOGGER_ERROR("{0}", message);
  if (severity == GL_DEBUG_SEVERITY_HIGH) {
    LOGGER_CRITICAL("Aborting...");
    abort();
  }
}

void glfw_impl::setup_initial_api_state(window_t &w) {
  static math::vec4 clear_color = {47.f / 255.f, 53.f / 255.f, 57.f / 255.f,
                                   1.00f};
#ifndef RELEASE_MODE
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(openglCallbackFunction, nullptr);
  glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR,
                        GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
#endif

  // glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

  int actualWindowWidth, actualWindowHeight;
  glfwGetWindowSize(w.get(), &actualWindowWidth, &actualWindowHeight);
  glViewport(0, 0, actualWindowWidth, actualWindowHeight);
  glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
}

void glfw_impl::key_callback(GLFWwindow *window, int key, int scancode,
                             int action, int mods) {
  input_state *input =
      reinterpret_cast<input_state *>(glfwGetWindowUserPointer(window));
  if (action == GLFW_PRESS) {
    input->keyboard.just_pressed.set(static_cast<size_t>(key));
    input->keyboard.pressed.set(static_cast<size_t>(key));
  } else if (action == GLFW_RELEASE) {
    input->keyboard.just_pressed.reset(static_cast<size_t>(key));
    input->keyboard.pressed.reset(static_cast<size_t>(key));
  }
}

mouse_state::mouse_button glfw_impl::mbutton_glfw_to_enum(int glfw_mbutton) {
  if (glfw_mbutton == GLFW_MOUSE_BUTTON_LEFT) {
    return mouse_state::mouse_button::left;
  } else if (glfw_mbutton == GLFW_MOUSE_BUTTON_RIGHT) {
    return mouse_state::mouse_button::right;
  } else if (glfw_mbutton == GLFW_MOUSE_BUTTON_MIDDLE) {
    return mouse_state::mouse_button::middle;
  }
  return mouse_state::mouse_button::other;
}

void glfw_impl::mouse_button_callback(GLFWwindow *w, int button, int action,
                                      int mods) {
  input_state *input =
      reinterpret_cast<input_state *>(glfwGetWindowUserPointer(w));

  double xpos, ypos;
  glfwGetCursorPos(w, &xpos, &ypos);

  if (action == GLFW_PRESS) {
    input->mouse.last_pos = {xpos, ypos};
    const auto pos = mbutton_glfw_to_enum(button);
    input->mouse.just_pressed.set(pos);
    input->mouse.pressed.set(pos);
  }

  if (action == GLFW_RELEASE) {
    const auto pos = mbutton_glfw_to_enum(button);
    input->mouse.last_pos = {xpos, ypos};
    input->mouse.just_pressed.reset(pos);
    input->mouse.pressed.reset(pos);
  }
}

void glfw_impl::mouse_move_callback(GLFWwindow *w, double xpos, double ypos) {
  input_state *input =
      reinterpret_cast<input_state *>(glfwGetWindowUserPointer(w));

  if (input->mouse.pressed[mouse_state::mouse_button::right]) {
    input->mouse.reoriented = {xpos, ypos};
  }
}

bool glfw_impl::set_keyboard_callbacks(window_t &w) {
  glfwSetKeyCallback(w.get(), glfw_impl::key_callback);
  return true;
}

bool glfw_impl::set_mouse_callbacks(window_t &w) {
  glfwSetMouseButtonCallback(w.get(), glfw_impl::mouse_button_callback);
  glfwSetCursorPosCallback(w.get(), glfw_impl::mouse_move_callback);
  return true;
}

void glfw_impl::initialize_api() {
  // setup GLFW
  glfwSetErrorCallback(glfw_impl::error_callback);
  atexit(glfw_impl::teardown);

  if (!glfwInit()) {
    glfw_impl::die("Couldn't initialize GLFW");
  }

  LOGGER_INFO("GLFW Successfully initialized");
}

bool glfw_impl::should_close(window_t &w) {
  return glfwWindowShouldClose(w.get());
} // namespace pusn

void glfw_impl::clear_color_and_depth(math::vec4 color, float depth) {
  glClearColor(color.x * color.w, color.y * color.w, color.z * color.w,
               color.w);
  glClearDepth(depth);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void glfw_impl::swap_buffers(window_t &w) { glfwSwapBuffers(w.get()); }

void glfw_impl::poll_events(window_t &w) { glfwPollEvents(); }

glfw_impl::window_t glfw_impl::initialize(const std::string &window_title,
                                          input_state *input) {
  static constexpr int init_w = 1600;
  static constexpr int init_h = 900;

  initialize_api();
  auto w = create_default_window(init_w, init_h, window_title.c_str());
  set_window_options(w, input);
  initialize_extensions();
  setup_initial_api_state(w);
  set_keyboard_callbacks(w);
  set_mouse_callbacks(w);
  return w;
}

void glfw_impl::before_frame() {
  static const math::vec4 clear_color = {47.f / 255.f, 53.f / 255.f,
                                         57.f / 255.f, 1.00f};
  static const float clear_depth = 1.f;
  clear_color_and_depth(clear_color, clear_depth);

  glfw_impl::last_frame_info::begin_time = glfwGetTimerValue();
}

void glfw_impl::after_frame(window_t &w) {
  static auto freq = glfwGetTimerFrequency();
  uint64_t end_time = glfwGetTimerValue();
  last_frame_info::last_frame_time =
      static_cast<double>(end_time - last_frame_info::begin_time) * 1000.f /
      freq;
  swap_buffers(w);
  poll_events(w);
}

GLuint compile_shader_from_source(const std::string &source, GLuint type) {
  GLuint shader = glCreateShader(type);
  const char *src = source.c_str();
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);

  GLint compiled;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (compiled != GL_TRUE) {
    GLsizei log_length = 0;
    GLchar message[1024];
    glGetShaderInfoLog(shader, 1024, &log_length, message);
    LOGGER_ERROR("[SHADER {0}] {1}", type, message);
  }

  return shader;
}

void glfw_impl::add_program_to_renderable(const std::string &program_name,
                                          renderable &out) {
  namespace fs = std::filesystem;
  // optional stages
  std::optional<GLuint> tesc_shader;
  std::optional<GLuint> tese_shader;

  std::string vert_source =
      utils::read_text_file((program_name + ".vert").c_str());
  std::string frag_source =
      utils::read_text_file((program_name + ".frag").c_str());

  if (fs::exists(program_name + ".tesc")) {
    std::string tesc_source =
        utils::read_text_file((program_name + ".tesc").c_str());
    tesc_shader =
        compile_shader_from_source(tesc_source, GL_TESS_CONTROL_SHADER);
  }

  if (fs::exists(program_name + ".tese")) {
    std::string tese_source =
        utils::read_text_file((program_name + ".tese").c_str());
    tese_shader =
        compile_shader_from_source(tese_source, GL_TESS_EVALUATION_SHADER);
  }

  GLuint vertex_shader =
      compile_shader_from_source(vert_source, GL_VERTEX_SHADER);
  GLuint frag_shader =
      compile_shader_from_source(frag_source, GL_FRAGMENT_SHADER);

  GLuint program = glCreateProgram();

  glAttachShader(program, vertex_shader);
  glAttachShader(program, frag_shader);

  if (tesc_shader.has_value() && tese_shader.has_value()) {
    glAttachShader(program, tesc_shader.value());
    glAttachShader(program, tese_shader.value());
  }

  glLinkProgram(program);

  GLint plinked;
  glGetProgramiv(program, GL_LINK_STATUS, &plinked);
  if (plinked != GL_TRUE) {
    GLsizei log_length = 0;
    GLchar message[1024];
    glGetProgramInfoLog(program, 1024, &log_length, message);
    LOGGER_ERROR("[PROG LINK] {0}", message);
  }

  glDeleteShader(vertex_shader);
  glDeleteShader(frag_shader);

  if (tesc_shader.has_value() && tese_shader.has_value()) {
    glDeleteShader(tesc_shader.value());
    glDeleteShader(tese_shader.value());
  }
  out.program = program;
}

void glfw_impl::use_program(GLuint program) { glUseProgram(program); }

void glfw_impl::render(const renderable &meta,
                       const api_agnostic_geometry &geom, render_mode mode) {
  glBindVertexArray(meta.vao.value());
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  if (mode == render_mode::triangles) {
    glDrawElements(GL_TRIANGLES, geom.indices.size(), GL_UNSIGNED_INT, NULL);
  } else if (mode == render_mode::patches) {
    glDrawElements(GL_PATCHES, geom.indices.size(), GL_UNSIGNED_INT, NULL);
  } else if (mode == render_mode::line_strip) {
    glLineWidth(4.f);
    glDrawElements(GL_LINE_STRIP, geom.indices.size(), GL_UNSIGNED_INT, NULL);
    glLineWidth(1.f);
  }
}

} // namespace pusn
