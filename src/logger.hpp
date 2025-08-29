#ifndef ESP_ASYNC_LOGGER_HPP
#define ESP_ASYNC_LOGGER_HPP

#include "device.hpp"
#include <memory>

#ifndef ASYNC_LOG_TASK_SIZE
#define ASYNC_LOG_TASK_SIZE 2048
#endif
#ifndef ASYNC_LOG_TASK_PRIORITY
#define ASYNC_LOG_TASK_PRIORITY tskIDLE_PRIORITY
#endif
#ifndef ASYNC_LOG_TASK_COREID
#define ASYNC_LOG_TASK_COREID tskNO_AFFINITY
#endif
#ifndef ASYNC_LOG_QUEUE_LENGTH
#define ASYNC_LOG_QUEUE_LENGTH 32
#endif


namespace asyncLogger
{
    /**
     * The main logging class
     */
    class Logger
    {
    public:
        using DeviceHandle = std::unique_ptr<Device>;

        Logger();

        /**
         * Registers a new logging device, i.e.,
         * creates a new device using the given arguments and appends it to the list of existing devices
         * @tparam TDevice The type of the logging device to register
         * @param level The log level the registered device should be associated with
         * @param format The format of the registered device
         * @param args Optional arguments passed to the constructor of the logging device
         * @return A handle to the newly created and registered logging device
         *         or an empty handle if the device could not be registered, i.e.,
         *         the initialization of the device failed
         */
        template <typename TDevice> requires std::is_base_of_v<Device, TDevice>
        const DeviceHandle& registerDevice(Level level = DEFAULT_LEVEL, int format = DEFAULT_FORMAT, auto&&... args);

        /**
         * Removes a logging device
         * @param handle The handle of the device to be removed from this logger
         */
        void unregisterDevice(const DeviceHandle& handle);
        /**
         * Logs a new entry
         * @param entry The entry to log
         */
        void log(const Entry& entry) const;
        /**
         * Logs a new entry with the file, line and function trace set to null values
         * @param level The level of this log entry
         * @param format The format string for the message of this log entry
         * @param args The arguments for the message format string
         */
        void log(Level level, const char* format, auto&&... args) const;
        /**
         * Logs a new entry in a save way to be called from inside an ISR (interrupt service routine)
         * @param entry The entry to log
         */
        void logISR(const Entry& entry) const;

    private:
        QueueHandle_t queue_{};
        StackType_t queue_stack_[ASYNC_LOG_QUEUE_LENGTH * sizeof(Entry)]{};
        StaticQueue_t queue_buf_{};
        TaskHandle_t task_{};
        StackType_t task_stack_[ASYNC_LOG_TASK_SIZE]{};
        StaticTask_t task_buf_{};
        Entry entry_buf_{};
        std::vector<DeviceHandle> devices_{};

        static inline const DeviceHandle empty_handle{};

        // logger task loop function
        [[noreturn]] static void loop(void* arg);
    };

    /**
     * Globally inlined logger instance
     */
    inline Logger Logger;


    template <typename TDevice> requires std::is_base_of_v<Device, TDevice>
    const Logger::DeviceHandle& Logger::registerDevice(Level level, int format, auto&&... args)
    {
        // only register the device if initialization succeeds
        if (DeviceHandle device = std::make_unique<TDevice>(level, format, std::forward<decltype(args)>(args)...);
            device != nullptr && device->initialize())
        {
            devices_.push_back(std::move(device));
            return devices_.back();
        }
        return empty_handle;
    }

    inline Logger::Logger()
    {
        // task and queue creation needs to be done inside the constructor function instead of using
        // a direct assignment to avoid referencing an incomplete 'this' inside the task loop function
        queue_ = xQueueCreateStatic(ASYNC_LOG_QUEUE_LENGTH, sizeof(Entry), queue_stack_, &queue_buf_);
        task_ = xTaskCreateStaticPinnedToCore(loop,
                                              "asyncLoggingTask",
                                              ASYNC_LOG_TASK_SIZE,
                                              this,
                                              ASYNC_LOG_TASK_PRIORITY,
                                              task_stack_,
                                              &task_buf_,
                                              ASYNC_LOG_TASK_COREID);
    }

    inline void Logger::unregisterDevice(const DeviceHandle& handle)
    {
        if (handle)
        {
            std::erase_if(devices_, [&handle](const DeviceHandle& ptr) { return ptr == handle; });
        }
    }

    inline void Logger::log(const Entry& entry) const
    {
        if (queue_)
        {
            xQueueSend(queue_, &entry, 0);
        }
    }

    void Logger::log(Level level, const char* format, auto&&... args) const
    {
        log({level, nullptr, 0, nullptr, format, std::forward<decltype(args)>(args)...});
    }

    inline void Logger::logISR(const Entry& entry) const
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        if (queue_)
        {
            xQueueSendFromISR(queue_, &entry, &xHigherPriorityTaskWoken);
        }
        if (xHigherPriorityTaskWoken)
        {
            portYIELD_FROM_ISR();
        }
    }

    inline void Logger::loop(void* arg)
    {
        for (;;)
        {
            if (auto logger = static_cast<Logger*>(arg);
                logger &&
                logger->queue_ &&
                xQueueReceive(logger->queue_, &logger->entry_buf_, portMAX_DELAY))
            {
                for (const auto& device : logger->devices_)
                {
                    device->write(logger->entry_buf_);
                }
            }
        }
    }
}


#endif //ESP_ASYNC_LOGGER_HPP
