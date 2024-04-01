//===- logrus.cpp - logrus log impl -----------------------------*- C++ -*-===//
//
/// \file
/// Reduce spdlog diffusion in header，Use spdlog::log interface to write log.
//
// Author:  zxh
// Date:    2024/03/30 22:07:29
//===----------------------------------------------------------------------===//

#include "logrus.h"

#define LOGRUS_LEVEL_NAME_CRITICAL spdlog::string_view_t("fatal", 5)

// Use "fatal" replace "critical"
#define SPDLOG_LEVEL_NAMES                                                     \
  {                                                                            \
    SPDLOG_LEVEL_NAME_TRACE, SPDLOG_LEVEL_NAME_DEBUG, SPDLOG_LEVEL_NAME_INFO,  \
        SPDLOG_LEVEL_NAME_WARNING, SPDLOG_LEVEL_NAME_ERROR,                    \
        LOGRUS_LEVEL_NAME_CRITICAL, SPDLOG_LEVEL_NAME_OFF                      \
  }

// Use "F"("fatal") replace "C" ("critical")
#define SPDLOG_SHORT_LEVEL_NAMES                                               \
  { "T", "D", "I", "W", "E", "F", "O" }

#include <spdlog/spdlog.h>

namespace logrus {

void logto(const char *file, int line, const char *func, Level level,
           const char *data, size_t n) {
  spdlog::log(spdlog::source_loc(file, line, func),
              (spdlog::level::level_enum)level, std::string_view(data, n));
  if (level == Level::kFatal)
    std::exit(1);
}

} // namespace logrus
