//
// Created by konradvonkirchbach on 5/27/20.
//

#ifndef MPIREORDERLIB_SRC_LOG_LOGGER_H
#define MPIREORDERLIB_SRC_LOG_LOGGER_H

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <memory>

namespace mpireorderinglib {
  class Logger {
  public:
    static void Init();
    static std::shared_ptr<spdlog::logger>& GetLogger();
    static void set_log_level(int lvl);

  private:
    static std::shared_ptr<spdlog::logger> m_Logger;
  };
}

//Logger Macros
#ifdef LOGGING
#define CARTREORDER_ERROR(x) mpireorderinglib::Logger::GetLogger()->error(x)
#define CARTREORDER_INFO(x) mpireorderinglib::Logger::GetLogger()->info(x)
#define CARTREORDER_WARN(x) mpireorderinglib::Logger::GetLogger()->warn(x)
#else
#define CARTREORDER_ERROR(...)
#define CARTREORDER_INFO(...)
#define CARTREORDER_WARN(...)
#endif

#endif //MPIREORDERLIB_SRC_LOG_LOGGER_H
