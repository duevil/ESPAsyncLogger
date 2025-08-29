#ifndef ESP_ASYNC_LOGGER_COMMON_H
#define ESP_ASYNC_LOGGER_COMMON_H


namespace asyncLogger
{
    enum class Level : uint8_t
    {
        NOTHING = 0,
        FATAL = 1,
        ERROR = 2,
        WARN = 3,
        NOTICE = 4,
        INFO = 5,
        DEBUG = 6,
        TRACE = 7,
        VERBOSE = 8,
        ALWAYS = 9
    };

    enum Format
    {
        LEVEL_LETTER = 0b01 << 0,
        LEVEL_SHORT = 0b10 << 0,
        LEVEL_FULL = 0b11 << 0,
        TIMESTAMP_SIMPLE = 0b01 << 2,
        TIMESTAMP_SHORT = 0b10 << 2,
        TIMESTAMP_FULL = 0b11 << 2,
        FILE_TRACE = 0b1 << 4,
        FUNCTION_TRACE = 0b1 << 5,
        TASK_TRACE = 0b1 << 6
    };

    constexpr auto LEVEL_STR_LETTER = "FEWNIDTVA";
    constexpr const char* LEVEL_STR_SHORT[] = {"FAT", "ERR", "WAR", "NOT", "INF", "DEB", "TRA", "VER", "ALW"};
    constexpr const char* LEVEL_STR_FULL[] = {
        "FATAL",
        "ERROR",
        "WARN",
        "NOTICE",
        "INFO",
        "DEBUG",
        "TRACE",
        "VERBOSE",
        "ALWAYS"
    };
    constexpr Level CORE_LEVEL_MAPPING[] = {
        Level::NOTHING,
        Level::ERROR,
        Level::WARN,
        Level::INFO,
        Level::DEBUG,
        Level::VERBOSE,
        Level::ALWAYS
    };
    constexpr auto DEFAULT_LEVEL =
#if defined USE_CORE_DEBUG_LEVEL && defined CORE_DEBUG_LEVEL
        CORE_LEVEL_MAPPING[CORE_DEBUG_LEVEL];
#elifdef ASYNC_LOG_DEFAULT_LEVEL
        static_cast<Level>(ASYNC_LOG_DEFAULT_LEVEL);
#else
        Level::WARN;
#endif
    constexpr auto DEFAULT_FORMAT = LEVEL_SHORT | TIMESTAMP_FULL | FILE_TRACE | FUNCTION_TRACE | TASK_TRACE;
}


#endif //ESP_ASYNC_LOGGER_COMMON_H
