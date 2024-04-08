#include "logrus.h"

#include <mutex>
#include <thread>

#include <benchmark/benchmark.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>

std::once_flag logrus_once;
std::once_flag spdlog_once;

static void BM_logrus(benchmark::State &state) {
  std::call_once(logrus_once, []() {
    logrus::set_rotating("default", "./default.log", 1024 * 1024 * 10, 3);
    logrus::set_pattern("%^%l%$ %Y%m%d %H:%M:%S %t  %v");
  });

  // Perform setup here
  for (auto _ : state)
    logrus::info("hello world");
}

static void BM_logrus_field(benchmark::State &state) {
  for (auto _ : state)
    logrus::with_field("ip", "127.0.0.1").info("New conn");
}

static void BM_logrus_fields(benchmark::State &state) {
  for (auto _ : state)
    logrus::with_fields({{"ip", "127.0.0.1"}}).info("New conn");
}

static void BM_logrus_fields_x2(benchmark::State &state) {
  for (auto _ : state)
    logrus::with_fields({{"ip", "127.0.0.1"}, {"port", 80}}).info("New conn");
}

static void BM_logrus_fields_x3(benchmark::State &state) {
  for (auto _ : state)
    logrus::with_fields(
        {{"ip", "127.0.0.1"}, {"port", 80}, {"netns", "netns_99"}})
        .info("New conn");
}

static void BM_spdlog(benchmark::State &state) {
  std::call_once(spdlog_once, []() {
    spdlog::set_default_logger(spdlog::rotating_logger_mt(
        "default1", "./default.log", 1024 * 1024 * 10, 3));
    spdlog::set_pattern("%^%l%$ %Y%m%d %H:%M:%S %t  %v");
  });

  for (auto _ : state)
    spdlog::info("hello world");
}

static void BM_spdlog_field(benchmark::State &state) {
  for (auto _ : state)
    spdlog::info("msg='New conn' ip='{}'", "127.0.0.1");
}

static void BM_spdlog_fields_x2(benchmark::State &state) {
  for (auto _ : state)
    spdlog::info("msg='New conn' ip='{}' port='{}'", "127.0.0.1", 80);
}

static void BM_spdlog_fields_x3(benchmark::State &state) {
  for (auto _ : state)
    spdlog::info("msg='New conn' ip='{}' port='{}' netns='{}'", "127.0.0.1", 80,
                 "netns_99");
}

BENCHMARK(BM_logrus);
BENCHMARK(BM_logrus_field);
BENCHMARK(BM_logrus_fields);
BENCHMARK(BM_logrus_fields_x2);
BENCHMARK(BM_logrus_fields_x3);

BENCHMARK(BM_spdlog);
BENCHMARK(BM_spdlog_field);
BENCHMARK(BM_spdlog_fields_x2);
BENCHMARK(BM_spdlog_fields_x3);

// Run the benchmark
BENCHMARK_MAIN();