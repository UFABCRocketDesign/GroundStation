#ifndef URD_TELEMETRY_GEN_H
#define URD_TELEMETRY_GEN_H

#pragma once

#include <Arduino.h>
#include "src\lib\URDCore\URDCore.h"

#if URD_TEST_ENABLE

class UrdTelemetryGenerator : public UrdBase
{
private:
    bool generatorStarted;

    unsigned long lastSendTime;
    unsigned long sendIntervalMs;

    unsigned long packetCounter;
    int testCase;

public:
    explicit UrdTelemetryGenerator(int baudRate = 115200);

    void setSendInterval(unsigned long intervalMs);
    void setTestCase(int testCaseConfig);

    void startHandshake() override;
    void update();

    String buildNominalPacket();
    String buildCorruptedPacket();

    bool isGeneratorStarted() const;

protected:
    bool processGpsCoords(const String& message);
};

#endif
#endif

