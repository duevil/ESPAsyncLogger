#include <Arduino.h>
#include <ESPAsyncLogger.h>


void setup()
{
    Logger.registerDevice<SerialLog>(Level::DEBUG, LEVEL_LETTER | TIMESTAMP_SHORT | FILE_TRACE | FUNCTION_TRACE);
    Logger.log(Level::INFO, "Hello World!");
}


void loop()
{
    Logger.log(Level::DEBUG, "Millis %lu", millis());

    delay(2000);
}
