#ifndef ESP_ASYNC_LOGGER_DEVICE_HPP
#define ESP_ASYNC_LOGGER_DEVICE_HPP

#include "entry.hpp"


namespace asyncLogger
{
    /**
     * Interface defining a generic logging device and extending the Arduino Print interface.
     *
     * Custom devices need to implement the <code>initialize</code> member function
     * and the member function defined by the Print interface
     * if the custom device's class doesn't already extend the Print interface.
     *
     * If required, an inheriting class can override the <code>writeStart</code> or <code>writeEnd</code>
     * member function to implement custom functionality before and/or after writing the log entry.
     *
     * If necessary, an inheriting class might override other <code>write[X]</code> member functions
     * to change the behavior of writing the different entry parts.
     */
    class Device : public Print
    {
    public:
        ~Device() override = default;
        /**
         * Initializes this logging device
         * @return Whether the initialization of the device was successful
         */
        virtual bool initialize() = 0;
        /**
         * Constructor
         * @param level The log level this device should use
         * @param format The log entry format this device should use
         */
        Device(Level level, uint8_t format) : level_(level), format_(format) {}
        /**
         * Changes the log level of this device
         * @param level The new level this device should use
         */
        void setLevel(Level level) { this->level_ = level; }
        /**
         * Changes the log format fof this device
         * @param format The new format this device should use
         */
        void setFormat(Format format) { this->format_ = format; }
        /**
         * Writes a log entry using the underlying Print interface
         * @param entry The entry to write
         */
        void write(const Entry& entry);

    protected:
        virtual void writeLevel(const Entry& entry);
        virtual void writeTimestamp(const Entry& entry);
        virtual void writeFileFunctionTrace(const Entry& entry);
        virtual void writeTaskTrace(const Entry& entry);
        virtual void writeStart(const Entry&) {}
        virtual void writeEnd(const Entry&) {}

    private:
        Level level_;
        uint8_t format_;

        Device& operator<<(auto x)
        {
            print(x);
            return *this;
        }
    };


    inline void Device::write(const Entry& entry)
    {
        if (level_ < entry.level)
        {
            return;
        }

        writeStart(entry);

        writeLevel(entry);
        writeTimestamp(entry);
        writeFileFunctionTrace(entry);
        writeTaskTrace(entry);

        if (format_)
        {
            *this << "- ";
        }

        println(entry.message);
        flush();

        writeEnd(entry);
    }

    inline void Device::writeLevel(const Entry& entry)
    {
        auto level = static_cast<std::underlying_type_t<Level>>(entry.level) - 1;
        if ((format_ & LEVEL_FULL) == LEVEL_FULL)
        {
            *this << '[' << LEVEL_STR_FULL[level] << "] ";
        }
        else if (format_ & LEVEL_LETTER)
        {
            *this << '[' << LEVEL_STR_LETTER[level] << "] ";
        }
        else if (format_ & LEVEL_SHORT)
        {
            *this << '[' << LEVEL_STR_SHORT[level] << "] ";
        }
    }

    inline void Device::writeTimestamp(const Entry& entry)
    {
        if ((format_ & TIMESTAMP_FULL) == TIMESTAMP_FULL)
        {
            // print timestamp as YYYY-MM-DD HH:MM:SS.sss
            tm tm{};
            gmtime_r(&entry.timestamp.tv_sec, &tm);
            char buf[32]{};
            char* tmp;
            asprintf(&tmp, "%%F %%T.%03lld ", entry.timestamp.tv_usec / 1000LL);
            strftime(buf, sizeof(buf), tmp, &tm);
            free(tmp);
            *this << buf;
        }
        else if (format_ & TIMESTAMP_SIMPLE)
        {
            // print timestamp in [seconds].[milliseconds]
            printf("%010lld.%03lld ", entry.timestamp.tv_sec, entry.timestamp.tv_usec / 1000LL);
        }
        else if (format_ & TIMESTAMP_SHORT)
        {
            // print timestamp as HH:MM:SS
            tm tm{};
            gmtime_r(&entry.timestamp.tv_sec, &tm);
            char buf[10]{};
            strftime(buf, sizeof(buf), "%T ", &tm);
            *this << buf;
        }
    }

    inline void Device::writeFileFunctionTrace(const Entry& entry)
    {
        if ((format_ & FILE_TRACE || format_ & FUNCTION_TRACE) && (entry.file || entry.function))
        {
            *this << '[';
            if (format_ & FILE_TRACE && entry.file)
            {
                *this << entry.file << ':' << entry.line;

                if (format_ & FUNCTION_TRACE && entry.function)
                {
                    *this << ' ';
                }
            }
            if (format_ & FUNCTION_TRACE && entry.function)
            {
                *this << entry.function;
            }
            *this << "] ";
        }
    }

    inline void Device::writeTaskTrace(const Entry& entry)
    {
        if (format_ & TASK_TRACE)
        {
            auto name = pcTaskGetName(entry.task);
            *this << "[task: " << (name ? name : "<null>") << "] ";
        }
    }
}


#endif //ESP_ASYNC_LOGGER_DEVICE_HPP
