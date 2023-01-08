#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace pusn {
struct logger {
  static bool init();

  inline static std::shared_ptr<spdlog::logger> &get_logger() {
    return core_logger;
  }

private:
  static std::shared_ptr<spdlog::logger> core_logger;
};

} // namespace pusn

// MACROS
#define LOGGER_TRACE(...) pusn::logger::get_logger()->trace(__VA_ARGS__)
#define LOGGER_INFO(...) pusn::logger::get_logger()->info(__VA_ARGS__)
#define LOGGER_WARN(...) pusn::logger::get_logger()->warn(__VA_ARGS__)
#define LOGGER_ERROR(...) pusn::logger::get_logger()->error(__VA_ARGS__)
#define LOGGER_CRITICAL(...) pusn::logger::get_logger()->critical(__VA_ARGS__)
