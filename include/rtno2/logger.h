#pragma once

#include <spdlog/spdlog.h>


#ifndef RTNO2API
#ifdef WIN32
#ifdef _WINDLL
#ifdef rtno_proxy_EXPORTS
#define RTNO2API __declspec(dllexport)
#else
#define RTNO2API __declspec(dllimport)
#endif
#else
#define RTNO2API
#endif
#else
#define RTNO2API
#endif
#endif

namespace ssr::rtno2 {

    using logger_t = spdlog::logger;

    enum class LOGLEVEL {
        TRACE = spdlog::level::trace,
        DEBUG = spdlog::level::debug,
        INFO = spdlog::level::info,
        WARN = spdlog::level::warn,
        ERR = spdlog::level::err,
        CRITICAL = spdlog::level::critical,
        NONE = spdlog::level::off,
    };
    void RTNO2API init_logger();
    logger_t RTNO2API get_logger(const std::string& name);
    void RTNO2API set_log_level(logger_t* logger, const LOGLEVEL level);

}

#define RTNO_TRACE(logger, ...) (logger).trace(__VA_ARGS__)
#define RTNO_DEBUG(logger, ...) (logger).debug(__VA_ARGS__)
#define RTNO_INFO(logger, ...) (logger).info(__VA_ARGS__)
#define RTNO_WARN(logger, ...) (logger).warn(__VA_ARGS__)
#define RTNO_ERROR(logger, ...) (logger).error(__VA_ARGS__)
#define RTNO_CRITICAL(logger, ...) (logger).critical(__VA_ARGS__)