//===- logrus.h - logrus for cpp --------------------------------*- C++ -*-===//
//
/// \file
/// A structured logging interface similar to logrus based on spdlog,
/// implemented using C++17.
//
// Author:  zxh
// Date:    2024/02/06 14:49:29
//===----------------------------------------------------------------------===//

#include <algorithm>
#include <variant>
#include <vector>

#include <fmt/format.h>

namespace logrus {

#define SPDLOG_LEVEL_TRACE 0
#define SPDLOG_LEVEL_DEBUG 1
#define SPDLOG_LEVEL_INFO 2
#define SPDLOG_LEVEL_WARN 3
#define SPDLOG_LEVEL_ERROR 4
#define SPDLOG_LEVEL_CRITICAL 5

enum Level : int {
  kTrace = SPDLOG_LEVEL_TRACE,
  kDebug = SPDLOG_LEVEL_DEBUG,
  kInfo = SPDLOG_LEVEL_INFO,
  kWarn = SPDLOG_LEVEL_WARN,
  kError = SPDLOG_LEVEL_ERROR,
  kFatal = SPDLOG_LEVEL_CRITICAL,
};

void logto(const char *file, int line, const char *func, Level level,
           const char *data, size_t n);

const char kFieldMsgKey[] = "msg";
const char kFieldErrKey[] = "error";
const char kFieldDelim[] = "=";
const char kFieldValueQuoted[] = "'";

template <size_t N>
constexpr size_t strlenConst(const char (&str)[N]) {
  return N - 1;
}

using ValueType =
    std::variant<int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t,
                 uint64_t, float, double, bool, std::string, const char *>;

template <typename T = ValueType>
class BasicEntry {
public:
  using FieldType = std::pair<std::string, T>;

  BasicEntry() {}

  BasicEntry(const std::initializer_list<FieldType> &fields)
      : fields_(fields) {}

  template <typename T1>
  BasicEntry(const std::string &k, const T1 &v) : BasicEntry({{k, v}}) {}

  template <typename T1>
  BasicEntry(const std::string &k, T1 &&v)
      : BasicEntry({{k, std::forward<T1>(v)}}) {}

  BasicEntry with_fields(const std::initializer_list<FieldType> &fields) const {
    BasicEntry entry;
    entry.fields_ = fields_;
    std::copy(fields.begin(), fields.end(), std::back_inserter(entry.fields_));
    return entry;
  }

  template <typename T1>
  BasicEntry with_field(const std::string &k, const T1 &v) const {
    return with_fields({{k, v}});
  }

  template <typename T1>
  BasicEntry with_field(const std::string &k, T1 &&v) const {
    return with_fields({{k, std::forward<T1>(v)}});
  }

#define LOGRUS_DECLARE_ENTRY_LOG(logfunc, level)                               \
  template <size_t N>                                                          \
  void logfunc(const char(&msg)[N]) const {                                    \
    return log("", 0, "", level, msg);                                         \
  }

  LOGRUS_DECLARE_ENTRY_LOG(trace, Level::kTrace);
  LOGRUS_DECLARE_ENTRY_LOG(debug, Level::kDebug);
  LOGRUS_DECLARE_ENTRY_LOG(info, Level::kInfo);
  LOGRUS_DECLARE_ENTRY_LOG(warn, Level::kWarn);
  LOGRUS_DECLARE_ENTRY_LOG(error, Level::kError);
  LOGRUS_DECLARE_ENTRY_LOG(fatal, Level::kFatal);
#undef LOGRUS_DECLARE_ENTRY_LOG

#define LOGRUS_DECLARE_ENTRY_LOG_WITH_LOC(logfunc, level)                      \
  template <size_t N>                                                          \
  void logfunc(const char *file, int line, const char *func,                   \
               const char(&msg)[N]) const {                                    \
    return log(file, line, func, level, msg);                                  \
  }

  LOGRUS_DECLARE_ENTRY_LOG_WITH_LOC(trace, Level::kTrace);
  LOGRUS_DECLARE_ENTRY_LOG_WITH_LOC(debug, Level::kDebug);
  LOGRUS_DECLARE_ENTRY_LOG_WITH_LOC(info, Level::kInfo);
  LOGRUS_DECLARE_ENTRY_LOG_WITH_LOC(warn, Level::kWarn);
  LOGRUS_DECLARE_ENTRY_LOG_WITH_LOC(error, Level::kError);
  LOGRUS_DECLARE_ENTRY_LOG_WITH_LOC(fatal, Level::kFatal);
#undef LOGRUS_DECLARE_ENTRY_LOG_WITH_LOC

private:
  template <size_t N>
  void log(const char *file, int line, const char *func, Level level,
           const char (&msg)[N]) const {
    fmt::basic_memory_buffer<char, 256> buf;

    // Handle msg field
    std::copy_n(kFieldMsgKey, strlenConst(kFieldMsgKey),
                std::back_inserter(buf));
    std::copy_n(kFieldDelim, strlenConst(kFieldDelim), std::back_inserter(buf));
    std::copy_n(kFieldValueQuoted, strlenConst(kFieldValueQuoted),
                std::back_inserter(buf));
    std::copy_n(msg, strlenConst(msg), std::back_inserter(buf));
    std::copy_n(kFieldValueQuoted, strlenConst(kFieldValueQuoted),
                std::back_inserter(buf));

    // Handle normal fields
    for (const auto &pair : fields_) {
      std::visit(
          [&](const auto &value) {
            std::string s =
                fmt::format(" {}={}{}{}", pair.first, kFieldValueQuoted, value,
                            kFieldValueQuoted);
            std::copy_n(s.begin(), s.size(), std::back_inserter(buf));
          },
          pair.second);
    }
    buf.push_back('\0');

    logto(file, line, func, level, buf.data(), buf.size());
  }

  template <size_t N>
  void log(Level level, const char (&msg)[N]) const {
    return log("", 0, "", level, msg);
  }

private:
  std::vector<FieldType> fields_;
};

using Entry = BasicEntry<>;

template <typename T>
inline Entry with_field(const std::string &k, const T &v) {
  return Entry(k, v);
}

template <typename T>
inline Entry with_field(const std::string &k, T &&v) {
  return Entry(k, std::forward<T>(v));
}

inline Entry
with_fields(const std::initializer_list<typename Entry::FieldType> &fields) {
  return Entry(fields);
}

inline Entry with_error(int errnum) {
  return Entry(kFieldErrKey, strerror(errnum));
}

#define LOGRUS_DECLARE_LOG_WITH_LOC(logfunc)                                   \
  template <size_t N>                                                          \
  void logfunc(const char *file, int line, const char *func,                   \
               const char(&msg)[N]) {                                          \
    return Entry{}.logfunc(file, line, func, msg);                             \
  }

LOGRUS_DECLARE_LOG_WITH_LOC(trace);
LOGRUS_DECLARE_LOG_WITH_LOC(debug);
LOGRUS_DECLARE_LOG_WITH_LOC(info);
LOGRUS_DECLARE_LOG_WITH_LOC(warn);
LOGRUS_DECLARE_LOG_WITH_LOC(error);
LOGRUS_DECLARE_LOG_WITH_LOC(fatal);
#undef LOGRUS_DECLARE_LOG_WITH_LOC

#define LOGRUS_DECLARE_LOG(logfunc)                                            \
  template <size_t N>                                                          \
  void logfunc(const char(&msg)[N]) {                                          \
    return Entry{}.logfunc(msg);                                               \
  }

LOGRUS_DECLARE_LOG(trace);
LOGRUS_DECLARE_LOG(debug);
LOGRUS_DECLARE_LOG(info);
LOGRUS_DECLARE_LOG(warn);
LOGRUS_DECLARE_LOG(error);
LOGRUS_DECLARE_LOG(fatal);
#undef LOGRUS_DECLARE_LOG

} // namespace logrus

#define KV(k, v)                                                               \
  { k, v }

#define KERR(errnum) KV(logrus::kFieldErrKey, strerror(errnum))

#ifdef LOGRUS_WITH_LOC
#define LOG_TRACE(msg, ...)                                                    \
  logrus::Entry({__VA_ARGS__}).trace(__FILE__, __LINE__, __FUNCTION__, msg)
#define LOG_DEBUG(msg, ...)                                                    \
  logrus::Entry({__VA_ARGS__}).debug(__FILE__, __LINE__, __FUNCTION__, msg)
#define LOG_INFO(msg, ...)                                                     \
  logrus::Entry({__VA_ARGS__}).info(__FILE__, __LINE__, __FUNCTION__, msg)
#define LOG_WARN(msg, ...)                                                     \
  logrus::Entry({__VA_ARGS__}).warn(__FILE__, __LINE__, __FUNCTION__, msg)
#define LOG_ERROR(msg, ...)                                                    \
  logrus::Entry({__VA_ARGS__}).error(__FILE__, __LINE__, __FUNCTION__, msg)
#define LOG_FATAL(msg, ...)                                                    \
  logrus::Entry({__VA_ARGS__}).fatal(__FILE__, __LINE__, __FUNCTION__, msg)
#else
#define LOG_TRACE(msg, ...) logrus::Entry({__VA_ARGS__}).trace(msg)
#define LOG_DEBUG(msg, ...) logrus::Entry({__VA_ARGS__}).debug(msg)
#define LOG_INFO(msg, ...) logrus::Entry({__VA_ARGS__}).info(msg)
#define LOG_WARN(msg, ...) logrus::Entry({__VA_ARGS__}).warn(msg)
#define LOG_ERROR(msg, ...) logrus::Entry({__VA_ARGS__}).error(msg)
#define LOG_FATAL(msg, ...) logrus::Entry({__VA_ARGS__}).fatal(msg)
#endif
