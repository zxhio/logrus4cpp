#include "logrus.h"

int main() {
  // Default logger.
  logrus::info("hello world!");

  logrus::with_field("port", 80).info("Listen on");
  logrus::with_field("addr", "127.0.0.1:80").info("Listen on");

  logrus::with_field("ip", "127.0.0.1")
      .with_field("port", 80)
      .info("Listen on");

  logrus::with_fields({{"ip", "127.0.0.1"}, {"port", 80}}).info("Listen on");

  logrus::Entry x = logrus::with_field("id", 1).with_fields(
      {{"ip", "127.0.0.1"}, {"port", 80}});
  x.with_field("proto", "udp").info("Listen on");
  x.with_field("proto", "tcp").info("Listen on");

  // Use macro to log.
  LOG_INFO("hello world!");
  LOG_INFO("Listen on", KV("port", 80));
  LOG_INFO("Listen on", KV("ip", "127.0.0.1"), KV("port", 80));

  logrus::with_error(EPERM).with_field("file", "foo").error("Fail to open");
  LOG_ERROR("Fail to open", KERR(EPERM), KV("file", "foo"));

  // Set logger format.
  logrus::set_pattern("%^%l%$ %Y%m%d %H:%M:%S %t  %v");
  logrus::set_level(logrus::kTrace);
  logrus::trace("hello world!");
  logrus::debug("hello world!");
  logrus::info("hello world!");

  LOG_INFO("hello world!");
  LOG_INFO("Listen on", KV("port", 80));
  LOG_INFO("Listen on", KV("ip", "127.0.0.1"), KV("port", 80));

  // Set file logger.
  logrus::set_rotating("default", "./default.log", 1024 * 1024 * 10, 3);
  logrus::set_pattern("%^%l%$ %Y%m%d %H:%M:%S %t  %v");
  logrus::info("hello world!");
  LOG_INFO("hello world!");

  logrus::Logger l;
  l.set_rotating("custom", "./custom.log", 1024 * 1024 * 10, 3);
  l.info("hello world!");
  LOG_INFO_(l, "hello world!");
}