#ifndef URD_TEST_H
#define URD_TEST_H

#pragma once

#include <Arduino.h>
#include "src\lib\URDCore\URDCore.h"

#if URD_TEST_ENABLE

class UrdGroundStationTest : public UrdBase
{
private:
    bool testStarted;

public:
    explicit UrdGroundStationTest(int baudRate = 115200);

    void startHandshake() override;
    void update();

    void sendTelemetryLine(const String& telemetryLine);

    bool isTestStarted() const;

protected:
    bool processGpsCoords(const String& message);
};

#endif
#endif

