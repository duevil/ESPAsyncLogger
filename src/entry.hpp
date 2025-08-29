#ifndef ESP_ASYNC_LOGGER_ENTRY_HPP
#define ESP_ASYNC_LOGGER_ENTRY_HPP

#include "common.h"


namespace asyncLogger
{
    constexpr auto MAX_MSG_LEN = 95;

    /**
     * Log entry structure
     */
    struct Entry
    {
        timeval timestamp{};
        const char* file;
        uint32_t line;
        const char* function;
        TaskHandle_t task;
        char message[MAX_MSG_LEN]{};
        Level level;

        /**
         * Creates a new empty log entry
         */
        Entry(): file(nullptr), line(0), function(nullptr), task(nullptr), level() {}

        /**
         * Creates a new log entry
         * @param level The level of this log entry
         * @param file The file trace of this log entry
         * @param line The line trace of this log entry
         * @param function The function trace of this log entry
         * @param format The format string for the message of this log entry
         * @param args The arguments for the message format string
         * @note The entry's message should be less than 95 characters long after formatting
         * @note The entry's timestamp is retrieved using <code>gettimeofday</code>
         */
        Entry(Level level, const char* file, uint32_t line, const char* function, const char* format, auto&&... args):
            file(file),
            line(line),
            function(function),
            task(xTaskGetCurrentTaskHandle()),
            level(level)
        {
            gettimeofday(&timestamp, nullptr);
            if (format)
            {
                snprintf(message, MAX_MSG_LEN, format, std::forward<decltype(args)>(args)...);
            }
        }
    };
}


#endif //ESP_ASYNC_LOGGER_ENTRY_HPP
