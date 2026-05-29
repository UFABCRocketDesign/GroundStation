#include "UrdGroundStationTest.h"

#if URD_TEST_ENABLE


UrdGroundStationTest::UrdGroundStationTest(int baudRate)
    : UrdBase(baudRate)
{
    testStarted = false;
}

void UrdGroundStationTest::startHandshake()
{
    appStarted = false;
    readyReceived = false;
    testStarted = false;

    Serial.println(F("Starting URD Ground Station Test..."));

    while (!appStarted)
    {
        String message;

        if (readAppMessage(message))
        {
            if (processReady(message)) { continue; }
            if (processReset(message)) { continue; }
            if (processGpsCoords(message)) { continue; }

            processUnknownMessage(message);
        }

        delay(10);
    }

    ledOn();
}

bool UrdGroundStationTest::processGpsCoords(const String& message)
{
    if (message != UrdProtocol::GPS_COORDS)
    {
        return false;
    }

    Serial.println(F("~\t~"));

    testStarted = true;
    markAppStarted();

    Serial.println(F("---Flight Computer RTOS---"));

    if (buzzerEnabled)
    {
        playDash();
    }

    return true;
}

void UrdGroundStationTest::update()
{
    // Este modo deixa o seu código externo decidir quando enviar a telemetria.
    // Use sendTelemetryLine(...).
}

void UrdGroundStationTest::sendTelemetryLine(const String& telemetryLine)
{
    if (!appStarted)
    {
        return;
    }

    Serial.println(telemetryLine);
}

bool UrdGroundStationTest::isTestStarted() const
{
    return testStarted;
}

#endif
