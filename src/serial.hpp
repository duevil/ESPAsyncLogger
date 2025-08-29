#ifndef ESP_ASYNC_LOGGER_SERIAL_HPP
#define ESP_ASYNC_LOGGER_SERIAL_HPP

#include "device.hpp"

#ifndef ASYNC_LOG_SERIAL_BAUD_RATE
#define ASYNC_LOG_SERIAL_BAUD_RATE 115200
#endif


namespace asyncLogger
{
    /**
     * Async logging device using the default Arduino Serial instance.
     *
     * By default the Serial port uses a baud rate of 115200, which can be changed by defining
     * <code>ASYNC_LOG_SERIAL_BAUD_RATE</code> with a different value before including this file.
     */
    class SerialLog final : public Device
    {
    public:
        SerialLog(Level level, int format, unsigned long baud_rate = ASYNC_LOG_SERIAL_BAUD_RATE):
            Device(level, format), baud_rate_(baud_rate) {}

        bool initialize() override
        {
            Serial.begin(baud_rate_);
            return true;
        }

        size_t write(uint8_t data) override
        {
            return Serial.write(data);
        }

        int availableForWrite() override
        {
            return Serial.availableForWrite();
        }

        void flush() override
        {
            Serial.flush();
        }

    private:
        unsigned long baud_rate_;
    };
}


#endif //ESP_ASYNC_LOGGER_SERIAL_HPP
