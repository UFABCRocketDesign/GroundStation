#include "UrdTelemetryGenerator.h"

#if URD_TEST_ENABLE


UrdTelemetryGenerator::UrdTelemetryGenerator(int baudRate)
    : UrdBase(baudRate)
{
    generatorStarted = false;

    lastSendTime = 0;
    sendIntervalMs = 200;

    packetCounter = 0;
    testCase = 0;
}

void UrdTelemetryGenerator::setSendInterval(unsigned long intervalMs)
{
    sendIntervalMs = intervalMs;
}

void UrdTelemetryGenerator::setTestCase(int testCaseConfig)
{
    testCase = testCaseConfig;
}

void UrdTelemetryGenerator::startHandshake()
{
    appStarted = false;
    readyReceived = false;
    generatorStarted = false;

    Serial.println(F("Starting URD Telemetry Generator..."));

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

bool UrdTelemetryGenerator::processGpsCoords(const String& message)
{
    if (message != UrdProtocol::GPS_COORDS)
    {
        return false;
    }

    Serial.println(F("~\t~"));

    generatorStarted = true;
    markAppStarted();

    Serial.println(F("---URD Telemetry Generator---"));

    if (buzzerEnabled)
    {
        playDash();
    }

    return true;
}

void UrdTelemetryGenerator::update()
{
    if (!appStarted)
    {
        return;
    }

    if ((millis() - lastSendTime) < sendIntervalMs)
    {
        return;
    }

    lastSendTime = millis();

    if (testCase == 0)
    {
        Serial.println(buildNominalPacket());
    }
    else
    {
        Serial.println(buildCorruptedPacket());
    }

    packetCounter++;
}

String UrdTelemetryGenerator::buildNominalPacket()
{
    float t = millis() / 1000.0f;
    float altitude = 10.0f + 0.5f * packetCounter;
    float pressure = 101325.0f - altitude * 12.0f;

    String packet;

    packet += "L:";
    packet += String(packetCounter);
    packet += '\t';

    packet += "T:";
    packet += String(t, 2);
    packet += '\t';

    packet += "LAT:";
    packet += String(0.0f, 6);
    packet += '\t';

    packet += "LON:";
    packet += String(0.0f, 6);
    packet += '\t';

    packet += "PR:";
    packet += String(pressure, 2);
    packet += '\t';

    packet += "H:";
    packet += String(altitude, 2);
    packet += '\t';

    packet += "R:";
    packet += String(0.0f, 2);
    packet += '\t';

    packet += "P:";
    packet += String(0.0f, 2);
    packet += '\t';

    packet += "Y:";
    packet += String(0.0f, 2);

    return packet;
}

String UrdTelemetryGenerator::buildCorruptedPacket()
{
    if (testCase == 1)
    {
        return String("L:") + String(packetCounter) + "\tT:BAD\tH:100";
    }

    if (testCase == 2)
    {
        return String("L:") + String(packetCounter) + "\tH:";
    }

    if (testCase == 3)
    {
        return String("CORRUPTED_PACKET");
    }

    return buildNominalPacket();
}

bool UrdTelemetryGenerator::isGeneratorStarted() const
{
    return generatorStarted;
}

#endif
