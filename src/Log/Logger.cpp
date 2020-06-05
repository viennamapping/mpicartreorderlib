//
// Created by konradvonkirchbach on 5/27/20.
//

#include "Logger.h"

namespace mpireorderinglib {
  std::shared_ptr<spdlog::logger> Logger::m_Logger;

  void Logger::Init() {
    spdlog::set_pattern("%^[%T:%l] %n: %v%$");
    m_Logger = spdlog::stdout_color_mt("CARTREORDER");
    m_Logger->set_level(spdlog::level::info);
  }

std::shared_ptr<spdlog::logger> &Logger::GetLogger() {
  return m_Logger;
}
}
void mpireorderinglib::Logger::set_log_level(int lvl) {
    switch (lvl) {
    case 0:
	  m_Logger->set_level(spdlog::level::trace);
	  break;
    case 1:
      m_Logger->set_level(spdlog::level::debug);
	  break;
    case 2:
      m_Logger->set_level(spdlog::level::info);
	  break;
    case 3:
      m_Logger->set_level(spdlog::level::warn);
	  break;
    case 4:
      m_Logger->set_level(spdlog::level::err);
	  break;
    case 5:
      m_Logger->set_level(spdlog::level::critical);
	  break;
    case 6:
      m_Logger->set_level(spdlog::level::off);
	  break;
    default:
      m_Logger->set_level(spdlog::level::off);
	}
}
