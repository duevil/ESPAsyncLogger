#include <Arduino.h>
#include <ESPAsyncLogger.h>


void setup()
{
    ALOG_REGISTER(SerialLog, Level::DEBUG, LEVEL_LETTER | TIMESTAMP_SHORT | FILE_TRACE | FUNCTION_TRACE);
    ALOG_I("Hello World!");
}


void loop()
{
    ALOG_D("Millis %lu", millis());

    delay(2000);
}
