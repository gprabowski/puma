#include <logger.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> pusn::logger::core_logger;

bool pusn::logger::init() {
  spdlog::set_pattern("%^[%T] %n: %v%$");
  core_logger = spdlog::stdout_color_mt("Milling LOG");
  core_logger->set_level(spdlog::level::trace);
  LOGGER_INFO("Initialized log!");
  return true;
}
