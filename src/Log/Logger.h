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
    inline static std::shared_ptr<spdlog::logger>& GetLogger();
    static void set_log_level(int lvl);

  private:
    static std::shared_ptr<spdlog::logger> m_Logger;
  };
}

//Logger Macros
#ifdef LOGGING
#define CARTREORDER_ERROR(...) ::mpireorderling::Logger::GetLogger()->error(_VA_ARGS__)
#define CARTREORDER_INFO(...) ::mpireorderling::Logger::GetLogger()->info(_VA_ARGS__)
#define CARTREORDER_WARN(...) ::mpireorderling::Logger::GetLogger()->warn(_VA_ARGS__)
#else
#define CARTREORDER_ERROR(...)
#define CARTREORDER_INFO(...)
#define CARTREORDER_WARN(...)
#endif

#endif //MPIREORDERLIB_SRC_LOG_LOGGER_H
