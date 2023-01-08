#pragma once

#include <geometry.hpp>
#include <inputs.hpp>
#include <logger.hpp>

#include <glfw_impl/common.hpp>
#include <glfw_impl/framebuffer.hpp>

namespace pusn {

namespace glfw_impl {

// graphics api utils
void before_frame();
void after_frame(window_t &w);
bool should_close(window_t &w);
void clear_color_and_depth(math::vec4 color, float depth);
void swap_buffers(window_t &w);
void poll_events(window_t &w);
void fill_renderable(std::vector<pos_norm_col> &vertices,
                     std::vector<unsigned int> &indices, renderable &out);
void add_program_to_renderable(const std::string &program_name,
                               renderable &out);
inline auto get_ticks() { return glfwGetTime(); }
void use_program(GLuint program);
void render(const renderable &meta, const api_agnostic_geometry &geom,
            render_mode mode = render_mode::triangles);

template <typename TextureDataType>
void fill_texture(texture_t &texture, int x, int y,
                  TextureDataType *values_to_fill) {
  if (!texture.has_value() ||
      (texture.has_value() && (texture.width != x || texture.height != y))) {
    GLuint tmp;
    glCreateTextures(GL_TEXTURE_2D, 1, &tmp);
    texture.index = tmp;
    // color
    glTextureParameteri(tmp, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(tmp, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(tmp, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(tmp, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    if constexpr (std::is_same_v<TextureDataType, math::vec3>) {
      glTextureStorage2D(tmp, 1, GL_RGB32F, x, y);
    }

    if constexpr (std::is_same_v<TextureDataType, math::char3>) {
      glTextureStorage2D(tmp, 1, GL_RGB8, x, y);
    }

    if constexpr (std::is_same_v<TextureDataType, float>) {
      glTextureStorage2D(tmp, 1, GL_R32F, x, y);
    }

    if constexpr (std::is_same_v<TextureDataType, unsigned char>) {
      glTextureStorage2D(tmp, 1, GL_RGB8, x, y);
    }

    if constexpr (std::is_same_v<TextureDataType, char>) {
      glTextureStorage2D(tmp, 1, GL_RGB8, x, y);
    }

    texture.index = tmp;
    texture.width = x;
    texture.height = y;
  }
  if constexpr (std::is_same_v<TextureDataType, math::vec3>) {
    glTextureSubImage2D(texture.value(), 0, 0, 0, x, y, GL_RGB, GL_FLOAT,
                        values_to_fill);
  }

  if constexpr (std::is_same_v<TextureDataType, math::char3>) {
    glTextureSubImage2D(texture.value(), 0, 0, 0, x, y, GL_RGB,
                        GL_UNSIGNED_BYTE, values_to_fill);
  }

  if constexpr (std::is_same_v<TextureDataType, float>) {
    glTextureSubImage2D(texture.value(), 0, 0, 0, x, y, GL_RED, GL_FLOAT,
                        values_to_fill);
  }

  if constexpr (std::is_same_v<TextureDataType, unsigned char>) {
    glTextureSubImage2D(texture.value(), 0, 0, 0, x, y, GL_RGB,
                        GL_UNSIGNED_BYTE, values_to_fill);
  }

  if constexpr (std::is_same_v<TextureDataType, char>) {
    glTextureSubImage2D(texture.value(), 0, 0, 0, x, y, GL_RGB,
                        GL_UNSIGNED_BYTE, values_to_fill);
  }
}

template <typename UniformType>
inline void set_uniform(const std::string &name, GLuint program,
                        const UniformType &value) {
  if constexpr (std::is_same_v<math::mat4, UniformType>) {
    glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE,
                       math::get_value_ptr(value));
  }

  if constexpr (std::is_same_v<math::vec3, UniformType>) {
    glUniform3f(glGetUniformLocation(program, name.c_str()), value.x, value.y,
                value.z);
  }
}

// utils
inline mouse_state::mouse_button mbutton_glfw_to_enum(int glfw_mbutton);

// api callbacks
void error_callback(int error, const char *description);
void APIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id,
                                     GLenum severity, GLsizei length,
                                     const GLchar *message,
                                     const void *userParam);

// input callbacks
void mouse_button_callback(GLFWwindow *w, int button, int action, int mods);
void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_move_callback(GLFWwindow *w, double xpos, double ypos);

// api initialization and utils
window_t initialize(const std::string &window_title, input_state *input);
void initialize_api();
void teardown();
void die(const char *message);
void window_destroyer(GLFWwindow *w);
void initialize_extensions();
void setup_initial_api_state(window_t &w);

// window creation and utils
window_t create_default_window(const int w, const int h, const char *title);
void set_window_options(window_t &w, input_state *input);
bool set_keyboard_callbacks(window_t &w);
bool set_mouse_callbacks(window_t &w);

} // namespace glfw_impl
} // namespace pusn
