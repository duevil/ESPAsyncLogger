#ifndef ESP_ASYNC_LOGGER_H
#define ESP_ASYNC_LOGGER_H

#include "logger.hpp"
#include "serial.hpp"


// define ASYNC_LOG_USE_MACROS to 0 to exclude macros
#ifndef ASYNC_LOG_USE_MACROS
#define ASYNC_LOG_USE_MACROS 1
#endif

#if ASYNC_LOG_USE_MACROS


#define ALOG_ENTRY(level, format, ...) asyncLogger::Entry{level, __FILE__, __LINE__, __func__, format __VA_OPT__(,) __VA_ARGS__}

#if defined ASYNC_LOG_USE_CORE_DEBUG_LEVEL && CORE_DEBUG_LEVEL < 1
#define ALOG_F(format, ...) do {} while (0)
#define ALOG_E(format, ...) do {} while (0)
#else
#define ALOG_F(format, ...) asyncLogger::Logger.log(ALOG_ENTRY(asyncLogger::Level::FATAL, format __VA_OPT__(,) __VA_ARGS__))
#define ALOG_E(format, ...) asyncLogger::Logger.log(ALOG_ENTRY(asyncLogger::Level::ERROR, format __VA_OPT__(,) __VA_ARGS__))
#endif

#if defined ASYNC_LOG_USE_CORE_DEBUG_LEVEL && CORE_DEBUG_LEVEL < 2
#define ALOG_W(format, ...) do {} while (0)
#define ALOG_N(format, ...) do {} while (0)
#else
#define ALOG_W(format, ...) asyncLogger::Logger.log(ALOG_ENTRY(asyncLogger::Level::WARN, format __VA_OPT__(,) __VA_ARGS__))
#define ALOG_N(format, ...) asyncLogger::Logger.log(ALOG_ENTRY(asyncLogger::Level::NOTICE, format __VA_OPT__(,) __VA_ARGS__))
#endif

#if defined ASYNC_LOG_USE_CORE_DEBUG_LEVEL && CORE_DEBUG_LEVEL < 3
#define ALOG_I(format, ...) do {} while (0)
#else
#define ALOG_I(format, ...) asyncLogger::Logger.log(ALOG_ENTRY(asyncLogger::Level::INFO, format __VA_OPT__(,) __VA_ARGS__))
#endif

#if defined ASYNC_LOG_USE_CORE_DEBUG_LEVEL && CORE_DEBUG_LEVEL < 4
#define ALOG_D(format, ...) do {} while (0)
#else
#define ALOG_D(format, ...) asyncLogger::Logger.log(ALOG_ENTRY(asyncLogger::Level::DEBUG, format __VA_OPT__(,) __VA_ARGS__))
#endif

#if defined ASYNC_LOG_USE_CORE_DEBUG_LEVEL && CORE_DEBUG_LEVEL < 5
#define ALOG_T(format, ...) do {} while (0)
#define ALOG_V(format, ...) do {} while (0)
#else
#define ALOG_T(format, ...) asyncLogger::Logger.log(ALOG_ENTRY(asyncLogger::Level::TRACE, format __VA_OPT__(,) __VA_ARGS__))
#define ALOG_V(format, ...) asyncLogger::Logger.log(ALOG_ENTRY(asyncLogger::Level::VERBOSE, format __VA_OPT__(,) __VA_ARGS__))
#endif

#define ALOG_A(format, ...) asyncLogger::Logger.log(ALOG_ENTRY(asyncLogger::Level::ALWAYS, format __VA_OPT__(,) __VA_ARGS__))
#define ALOG(format, ...) ALOG_A(format __VA_OPT__(,) __VA_ARGS__)

#define ALOG_REGISTER(device_type, ...) asyncLogger::Logger.registerDevice<device_type>(__VA_ARGS__)


#endif


// define ASYNC_LOG_USE_NAMESPACE to 0 to exclude the asyncLogger namespace from the global namespace
#ifndef ASYNC_LOG_USE_NAMESPACE
#define ASYNC_LOG_USE_NAMESPACE 1
#endif
#if ASYNC_LOG_USE_NAMESPACE
using namespace asyncLogger;
#endif


#endif //ESP_ASYNC_LOGGER_H
