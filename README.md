# logrus4cpp

A structured logging interface similar to logrus based on spdlog, implemented using C++17.

## Usage

Only msg log.
```cpp
// info 20240207 15:08:15 msg='hello world!'
logrus::info("hello world!");
```

Contains single field log.
```cpp
// info 20240207 15:08:15 msg='Test c str' key1='xyz'
logrus::with_field("key1", "xyz").info("Test c str");

// info 20240207 15:08:15 msg='Test int' key3='1'
logrus::with_field("key3", 1).info("Test int");
```

Contains multiple fields log.
```cpp
// info 20240207 15:10:58 msg='with_fieldx2' ip='127.0.0.1' port='80'
logrus::with_field("ip", "127.0.0.1").with_field("port", 80).info("with_fieldx2");

// info 20240207 15:10:58 msg='with_fields' ip='127.0.0.1' port='80'
logrus::with_fields(logrus::Field("ip", "127.0.0.1"), logrus::Field("port", 80)).info("with_fields");
```

Reuse fields to logging.
```cpp
auto l = logrus::with_field("task_id", 1);

// info 20240207 15:10:58 msg='Listen on' task_id='1' ip='127.0.0.1' port='80'
l.with_fields(logrus::Field("ip", "127.0.0.1"), logrus::Field("port", 80)).info("Listen on");

// info 20240207 15:10:58 msg='Listen on' task_id='1' path='xx.sock'
l.with_field("path", "xx.sock").info("Listen on");
```

If think the code is long, can use macros.
```cpp
// info 20240207 15:18:02 msg='Task done'
LOG_INFO("Task done");

// info 20240207 15:10:58 msg='New conn' addr='127.0.0.1:80'
LOG_INFO("New conn", KV("addr", "127.0.0.1:80"));

// info 20240207 15:10:58 msg='Updated version' from='1.6.1' to='2.0.0' task_id='2'
LOG_INFO("Updated version", KV("from", "1.6.1"), KV("to", "2.0.0"), KV("task_id", 2));
```

## TODO
1. Add custom *Formatter*.
2. Add *spdlog::logger* option.
3. Improve the function definitions of *const T &* and *T &&*