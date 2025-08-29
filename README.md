# ESPAsyncLogger

Asynchronous logging library for the ESP32 using FreeRTOS queues and tasks to process logs in the background to avoid
interrupting time-critical program flows or heavy-duty tasks.

## Overview

- Asynchronous logging using FreeRTOS tasks and queues to process log entries in the background
- Logger instance can handle multiple logging devices to allow for different log processors
- Abstract logging device interface for creating custom log processors
- Custom macros to provide a logging API similar to the Arduino or ESP logging APIs
- Detailed log entries with timestamp and trace for file, line, function and task
- Individually configurable output format and log level for each logging device
- Diverse log levels: _Fatal_, _Error_, _Warning_, _Notice_, _Info_, _Debug_, _Trace_ and _Verbose_
- Easily configurable using preprocessor variables

## Why?

When using an ESP32 for more complex scenarios (e.g., webserver, UI drawing, and audio processing at the same time),
adding log is often necessary to discover bugs or to keep an eye on critical operations. While performing time-critical
or heavy-duty tasks, also writing logs to e.g., the Serial output or an SD card, can lead to performance issues.

This library circumvents this problem by queuing log entries to be processed in the background by an asynchronous task
only once the CPU has available resources.

## Compatibility

This library uses FreeRTOS features included in the ESP32 framework. Compatibility with other plattform was not (and
probably will never be) tested or realized.

Additionally, this library was developed with
the [pioarduino ESP32 core](https://github.com/pioarduino/platform-espressif32/). Compatibility with the default
PlatformIO core is not tested.

## Usage

To use the async logger, a new logging device needs to bre registered first, e.g., the `SerialLog` device which writes
to the default Arduino Serial output:

```c++
void setup() {
  // using macro
  ALOG_REGISTER(SerialLog);
  
  // using function
  Logger.registerDevice<SerialLog>();
} 
```

To queue a new log entry, it is recommended to use the predefined macros:

```c++
void loop() {
  // ...
  
  ALOG_D("Millis: %lu", millis());
  
  // ...
}
```

If the macro usage is not wanted, a log entry can be queued using the Logger methods, which is more verbose:

```c++
void loop() {
  // ...
  
  // this will set the file, line and fucntion trace values to null values:
  Logger.log(Level::INFO,  "Millis: %lu", millis());
  // or with trace values, but more verbose:
  Logger.log({Level::INFO, __FILE__, __LINE__, __FUNCTION__, "Millis: %lu", millis()});
  
  // it is also possible to use the predefined helper macro for creating a new entry:
  Logger.log(ALOG_ENTRY(Level::INFO, "Millis: %lu", millis()));
  
  // ...
}
```

If creating a log entry from insided an ISR (interrupt service routine) is needed, the special logger function can be
used:

```c++
void some_ISR() {
  Logger.logISR(ALOG_ENTRY(Level::INFO, "From ISR: %lu", millis());
}
```

## Format

This library provides multiple configurable log formats for the log level and file, function and task trace.
The format can be configured individually when registering a new logging device. The different format options are:

- **Log Level** (use one)
    - `LEVEL_LETER` – Write only the first character of the log level
    - `LEVEL_SHORT` – Write the first three letters of the log level
    - `LEVEL_FULL` – Write the full log level name (variable length)
- **Timestamp** (use one)
    - `TIMESTAMP_SIMPLE` – Write the timestamp in the format `[seconds].[milliseconds]`
    - `TIMESTAMP_SHORT` – Write timestamp in the format `HH:MM:SS`
    - `TIMESTAMP_FULL` – Write the timestamp in the format `YYYY-MM-DD HH:MM:SS.sss`
- **Tracing** (use any)
    - `FILE_TRACE` – Enable tracing for the file the log entry is called in
    - `FUNCTION_TRACE` – Enable tracing for the function name and line the log entry is called from
    - `TASK_TRACE` – Enable tracing for the ESP FreeRTOS task the log entry is called from

When registering a new logging device, the default format is
`LEVEL_SHORT | TIMESTAMP_FULL | FILE_TRACE | FUNCTION_TRACE | TASK_TRACE`. A custom format can be created by combining
the aforementioned constants or by modifying the default format: `DEFAULT_FORMAT ^ LEVEL_SHORT | LEVEL_LETTER` (instead
of `LEVEL_SHORT`, use `LEVEL_LETTER` as the level format)

## Macros

The library provides different macros for an easy-to-use logging API:

- Printf-like macros for writing a log entry:
  ```c++
  // format - printf-style format string
  // args - variadic argument list for the format string
  #define ALOG(format, /* args */ ...)
  ```
    - `ALOG_F` – Fatal
    - `ALOG_E` – Error
    - `ALOG_W` – Warning
    - `ALOG_N` – Notice
    - `ALOG_I` – Information
    - `ALOG_D` – Debug
    - `ALOG_T` – Trace
    - `ALOG_V` – Verbose
    - `ALOG_A` – Always (same as `ALOG`)
- Macro for easily registering new logging devices:
  ```c++
  // device type - The typename/class of the logging device to be registred 
  //               (e.g. asyncLogger::SerialLog)
  // args - Optional arguments for device registration and constrcution
  #define ALOG_REGISTER(device_type, /* args */ ...)
  ```
  If optional arguments need to be specified, the first two arguments must be the log level (first)
  and the log format (second) for the device to be registered:
  ```c++
  void setup() {
    ALOG_REGISTER(SerialLog, Level::DEBUG, 
                  LEVEL_SHORT | TIMESTAMP_SHORT | FILE_TRACE | FUNCTION_TRACE);
    // ...
  } 
  ```

If the macros are not needed or are conflicting with other macros of the same name `ASYNC_LOG_USE_MACROS` can be defined
to be 0 before including the library to undefine this library's macros (or add `-D ASYNC_LOG_USE_MACROS=0` to the
PlatformIO build flags).

## Configuration

The library uses multiple macros for configuring the resources used by the logger:

- `ASYNC_LOG_TASK_SIZE` (default `2048`) – The size in bytes of the static task stack the logs a processed in. For a
  more complex logging device, the default task stack size might not be enough. It can be increased by defining this
  macro with a bigger value before including the library (or by defining it as a build flag in the PlatformIO ini).
- `ASYNC_LOG_TASK_PRIORITY` (default `tskIdlePriority`) – The priority of the logger task. Bigger values might help to
  ensure faster log processing with the cost of increased interruption of the main loop task.
- `ASYNC_LOG_TASK_COREID` (default `tskNO_AFFINITY`) – The ID of ESP32's core the logger task should be pinned to. By
  default, the task uses either core depending on the current CPU usage, but can be changed to e.g. `APP_CPU_NUM` (Core
  1 or the core the Arduino task runs on) or `PRO_CPU_NUM` (Core 0 or the core on which e.g., the network task runs on).
- `ASYNC_LOG_QUEUE_LENGTH` (default `32`) – The number of log entries that can be queued at once. If many entries must
  be written at once, the queue length can be increased to ensure the logging device(s) can process all entries.
- `ASYNC_LOG_SERIAL_BAUD_RATE` (default 115200) – The baud rate used by the `SerialLog` device.
- `ASYNC_LOG_USE_NAMESPACE` (default `1`) – If declarations of the async logger library are conflicting with other
  declarations, this macro can be set to 0 to disable using the `asyncLogger` namespace in the global namespace.
- `ASYNC_LOG_USE_MACROS` (default `1`) – see above.

### Integration with native logging APIs

To integrate the macros with the native APIs, the macro `ASYNC_LOG_USE_CORE_DEBUG_LEVEL` can be used together with
`CORE_DEBUG_LEVEL` set to the wanted native level to automatically activate/deactivate the corresponding async logging
macros

## Custom logging devices

By using an abstract logging device interface, it is possible to create a custom logging device. Doing so only requires
inheriting from the abstract `asyncLogger::Device` class and implementing the `asyncLogger::Device::initialize`
function. If needed, custom functionality can be added by overriding additional member function (see documentation
inside `device.hpp`) 