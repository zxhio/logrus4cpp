# logrus4cpp

A structured logging interface similar to logrus based on spdlog, implemented using C++17.

## Usage

Only msg log.
```cpp
// [2024-04-01 10:27:33.063] [info] msg='hello world!'
logrus::info("hello world!");
```

Contains single field log.
```cpp
// [2024-04-01 10:27:33.063] [info] msg='Listen on' port='80'
logrus::with_field("port", 80).info("Listen on");

// [2024-04-01 10:27:33.063] [info] msg='Listen on' addr='127.0.0.1:80'
logrus::with_field("addr", "127.0.0.1:80").info("Listen on");
```

Contains multiple fields log.
```cpp
// [2024-04-01 10:27:33.063] [info] msg='Listen on' ip='127.0.0.1' port='80'
logrus::with_field("ip", "127.0.0.1").with_field("port", 80).info("Listen on");

// [2024-04-01 10:27:33.063] [info] msg='Listen on' ip='127.0.0.1' port='80'
logrus::with_fields({{"ip", "127.0.0.1"}, {"port", 80}}).info("Listen on");
```

Reuse fields to logging.
```cpp
logrus::Entry l = logrus::with_field("id", 1).with_fields({{"ip", "127.0.0.1"}, {"port", 80}});

// [2024-04-01 10:51:06.381] [info] msg='Listen on' id='1' ip='127.0.0.1' port='80' proto='udp'
l.with_field("proto", "udp").info("Listen on");

// [2024-04-01 10:51:06.381] [info] msg='Listen on' id='1' ip='127.0.0.1' port='80' proto='tcp'
l.with_field("proto", "tcp").info("Listen on");
```

Error field handle.
```cpp
// [2024-04-01 11:44:41.640] [error] msg='Fail to open' error='Operation not permitted' file='foo'
logrus::with_error(errno).with_field("file", "foo").error("Fail to open");
```

If think the code is long, can use macros.
```cpp
// Use macro to log.

// [2024-04-01 10:51:06.381] [info] msg='hello world!'
LOG_INFO("hello world!");

// [2024-04-01 10:51:06.381] [info] msg='Listen on' port='80'
LOG_INFO("Listen on", KV("port", 80));

// [2024-04-01 10:51:06.381] [info] msg='Listen on' ip='127.0.0.1' port='80'
LOG_INFO("Listen on", KV("ip", "127.0.0.1"), KV("port", 80));

// [2024-04-01 11:19:00.537] [fatal] msg='Fail to listen' ip='127.0.0.1' port='80'
LOG_FATAL("Fail to listen", KV("ip", "127.0.0.1"), KV("port", 80));

// [2024-04-01 11:44:41.640] [error] msg='Fail to open' error='Operation not permitted' file='foo'
LOG_ERROR("Fail to open", KERR(errno), KV("file", "foo"));
```

**Recommend using macros to make your logs more structured.**

## TODO
1. Add custom *Formatter*.
2. Add *spdlog::logger* option.