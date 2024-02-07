//===- logrus.h - logrus for cpp --------------------------------*- C++ -*-===//
//
/// \file
/// A structured logging interface similar to logrus based on spdlog,
/// implemented using C++17.
//
// Author:  zxh
// Date:    2024/02/06 14:49:29
//===----------------------------------------------------------------------===//

#pragma once

#include <spdlog/spdlog.h>

namespace logrus {

template <size_t N> struct Literal {
  constexpr Literal(const char (&literal)[N])
      : Literal(literal, std::make_index_sequence<N>{}) {}

  constexpr Literal(const Literal<N> &literal) : Literal(literal.s) {}

  template <size_t N1, size_t... I1, size_t N2, size_t... I2>
  constexpr Literal(const char (&str1)[N1], std::index_sequence<I1...>,
                    const char (&str2)[N2], std::index_sequence<I2...>)
      : s{str1[I1]..., str2[I2]..., '\0'} {}

  template <size_t... I>
  constexpr Literal(const char (&str)[N], std::index_sequence<I...>)
      : s{str[I]...} {}

  char s[N];
};

template <size_t N> constexpr auto make_literal(const char (&str)[N]) {
  return Literal(str);
}

template <size_t N> constexpr auto make_literal(const Literal<N> &literal) {
  return Literal(literal);
}

template <size_t N1, size_t N2>
constexpr auto make_literal(const char (&str1)[N1], const char (&str2)[N2]) {
  return Literal<N1 + N2 - 1>(str1, std::make_index_sequence<N1 - 1>{}, str2,
                              std::make_index_sequence<N2 - 1>{});
}

template <size_t N1, size_t N2>
constexpr auto make_literal(const Literal<N1> &literal1,
                            const Literal<N2> &literal2) {
  return make_literal(literal1.s, literal2.s);
}

template <size_t N1, size_t N2>
constexpr auto make_literal(const char (&str)[N1], const Literal<N2> &literal) {
  return make_literal(str, literal.s);
}

template <size_t N1, size_t N2>
constexpr auto make_literal(const Literal<N1> &literal, const char (&str)[N2]) {
  return make_literal(literal.s, str);
}

template <size_t N1, typename... Args>
constexpr auto make_literal(const char (&str)[N1], const Args &...args) {
  return make_literal(str, make_literal(args...));
}

template <size_t N1, typename... Args>
constexpr auto make_literal(const Literal<N1> &literal, const Args &...args) {
  return make_literal(literal, make_literal(args...));
}

template <size_t N, typename T> struct Field {
  Literal<N> key;
  T value;

  constexpr Field(const char (&k)[N], T &&v)
      : key(k), value(std::forward<T>(v)) {}

  constexpr Field(const Literal<N> &k, T &&v)
      : key(k), value(std::forward<T>(v)) {}

  constexpr Field(const char (&k)[N], const T &v) : key(k), value(v) {}

  constexpr Field(const Literal<N> k, const T &v) : key(k), value(v) {}
};

template <size_t N, typename T> Field(const char (&)[N], T) -> Field<N, T>;

template <size_t N, typename... Args> struct Formatter {
  Literal<N> fmt;
  std::tuple<Args...> args;

  Formatter(const Literal<N> &fmt, const std::tuple<Args...> &args)
      : fmt(fmt), args(args) {}

  Formatter(const Literal<N> &fmt, std::tuple<Args...> &&args)
      : fmt(fmt), args(std::forward<std::tuple<Args...>>(args)) {}

  void log(spdlog::level::level_enum level) {
    std::apply(
        [&](Args &&...args) {
          spdlog::log(level, fmt.s, std::forward<Args>(args)...);
        },
        std::forward<std::tuple<Args...>>(args));
  }
};

template <size_t N, typename T>
constexpr auto make_format_args(const Field<N, T> &field) {
  return Formatter<N + 5, T>(make_literal(field.key, "='{}'"), field.value);
}

template <size_t N1, typename T1, size_t N2, typename... Args>
constexpr auto make_format_args(const Field<N1, T1> &field,
                                const Formatter<N2, Args...> &formatter) {
  return Formatter<N1 + N2 + 5, T1, Args...>(
      make_literal(field.key, "='{}' ", formatter.fmt),
      std::tuple_cat(std::tie(field.value), formatter.args));
}

template <size_t N1, typename T1, size_t N2, typename... Args>
constexpr auto make_format_args(const Field<N1, T1> &field,
                                Formatter<N2, Args...> &&formatter) {
  return Formatter<N1 + N2 + 5, T1, Args...>(
      make_literal(field.key, "='{}' ", formatter.fmt),
      std::tuple_cat(std::tie(field.value), formatter.args));
}

template <size_t N1, typename T1, typename... Fields>
constexpr auto make_format_args(const Field<N1, T1> &field,
                                Fields &&...fileds) {
  return make_format_args(field,
                          make_format_args(std::forward<Fields>(fileds)...));
}

template <typename Tuple, size_t... Idx>
constexpr auto make_formatter(const Tuple &tpl, std::index_sequence<Idx...>) {
  return make_format_args(std::get<Idx>(tpl)...);
}

template <typename... Fields> struct Entry {
  std::tuple<Fields...> fields;

  template <size_t N, typename T>
  constexpr Entry(const Field<N, T> &field) : fields(std::make_tuple(field)) {}

  constexpr Entry(std::tuple<Fields...> &&fields) : fields(fields) {}

  constexpr Entry(const std::tuple<Fields...> &fields) : fields(fields) {}

  template <size_t N, typename T>
  constexpr auto with_field(const char (&k)[N], const T &v) {
    return with_fields(Field(k, v));
  }

  template <typename... Fields1>
  constexpr auto with_fields(const Fields1 &...fields1) {
    return Entry<Fields..., Fields1...>(
        std::tuple_cat(fields, std::tie(fields1...)));
  }

  template <size_t N1>
  void log(const char (&msg)[N1], spdlog::level::level_enum lvl) {
    make_formatter(std::tuple_cat(std::make_tuple(Field("msg", msg)), fields),
                   std::make_index_sequence<sizeof...(Fields) + 1>{})
        .log(lvl);
  }

  template <size_t N1> void trace(const char (&msg)[N1]) {
    log(msg, spdlog::level::trace);
  }

  template <size_t N1> void debug(const char (&msg)[N1]) {
    log(msg, spdlog::level::debug);
  }

  template <size_t N1> void info(const char (&msg)[N1]) {
    log(msg, spdlog::level::info);
  }

  template <size_t N1> void warn(const char (&msg)[N1]) {
    log(msg, spdlog::level::warn);
  }

  template <size_t N1> void error(const char (&msg)[N1]) {
    log(msg, spdlog::level::err);
  }

  template <size_t N1> void fatal(const char (&msg)[N1]) {
    log(msg, spdlog::level::critical);
    std::exit(1);
  }
};

template <size_t N, typename T>
Entry(const Field<N, T> &field) -> Entry<Field<N, T>>;

template <size_t N, typename T>
constexpr auto with_field(const char (&k)[N], const T &v) {
  return Entry(Field(k, v));
}

template <size_t N, typename T, typename... Fields>
constexpr auto with_fields(const Field<N, T> &field, const Fields &...fields) {
  return Entry(std::make_tuple(field, fields...));
}

template <size_t N, typename... Fields>
void trace(const char (&msg)[N], const Fields &...fields) {
  Entry(std::forward_as_tuple(fields...)).trace(msg);
}

template <size_t N, typename... Fields>
void debug(const char (&msg)[N], const Fields &...fields) {
  Entry(std::forward_as_tuple(fields...)).debug(msg);
}

template <size_t N, typename... Fields>
void info(const char (&msg)[N], const Fields &...fields) {
  Entry(std::forward_as_tuple(fields...)).info(msg);
}

template <size_t N, typename... Fields>
void warn(const char (&msg)[N], const Fields &...fields) {
  Entry(std::forward_as_tuple(fields...)).warn(msg);
}

template <size_t N, typename... Fields>
void error(const char (&msg)[N], const Fields &...fields) {
  Entry(std::forward_as_tuple(fields...)).error(msg);
}

template <size_t N, typename... Fields>
void fatal(const char (&msg)[N], const Fields &...fields) {
  Entry(std::forward_as_tuple(fields...)).fatal(msg);
}

} // namespace logrus

#define KV(k, v) logrus::Field(k, v)

#define LOG_TRACE(...) logrus::trace(__VA_ARGS__)
#define LOG_DEBUG(...) logrus::debug(__VA_ARGS__)
#define LOG_INFO(...) logrus::info(__VA_ARGS__)
#define LOG_WARN(...) logrus::warn(__VA_ARGS__)
#define LOG_ERROR(...) logrus::error(__VA_ARGS__)
#define LOG_FATAL(...) logrus::fatal(__VA_ARGS__)
