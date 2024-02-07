#include "logrus.h"

#include <vector>

int main() {
  spdlog::set_pattern("%^%4!l%$ %Y%m%d %H:%M:%S %v");

  logrus::info("hello world!");
  logrus::warn("hello 2024");
  logrus::debug("2023, bye!");

  logrus::with_field("key1", "xyz").info("Test c str");
  logrus::with_field("key2", std::string("std::string")).info("Test std::string");
  logrus::with_field("key3", 1).info("Test int");
  logrus::with_field("key4", 3.14).info("Test double");
  logrus::with_field("key5", 'c').info("Test char");

  logrus::with_field("ip", "127.0.0.1").with_field("port", 80).info("with_fieldx2");
  logrus::with_fields(logrus::Field("ip", "127.0.0.1"), logrus::Field("port", 80)).info("with_fields");
  logrus::with_fields(logrus::Field("ip", "127.0.0.1"), logrus::Field("port", 80)).with_field("proto", "tcp").info("with_fields+with_field");

  auto l = logrus::with_field("task_id", 1);
  l.with_fields(logrus::Field("ip", "127.0.0.1"), logrus::Field("port", 80)).info("Listen on");
  l.with_field("path", "xx.sock").info("Listen on");

  LOG_INFO("Task done");
  LOG_INFO("New conn", KV("addr", "127.0.0.1:80"));
  LOG_INFO("Updated version", KV("from", "1.6.1"), KV("to", "2.0.0"), KV("task_id", 2));
}