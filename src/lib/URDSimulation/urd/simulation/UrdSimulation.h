#ifndef URD_SIMULATOR_H
#define URD_SIMULATOR_H

#pragma once

#include <Arduino.h>
#include <math.h>
#include "src\lib\URDCore\URDCore.h"

#if URD_SIMULATION_ENABLE

class UrdSimulation : public UrdBase
{
private:
    bool simulationStarted;

    float pressure;
    float height;
    float initialHeight;
    float initialPressure;
    float maxHeight;

public:
    explicit UrdSimulation(int baudRate = 115200);

    void startHandshake() override;

    bool update();
    bool updateSimulation();

    bool isSimulationStarted() const;

    float getInitialPressure();
    float getInitialHeight();
    float getMaxHeight();
    float getPressure();
    float getHeight();

    float convertPressureToHeight(float pressureValue);
    float getInitialHeight(float pressureValue);

    void restartInitialPressure(float pressureReference, float heightReference);

    String getDataString();
    String getHeaderString();

protected:
    bool processSimulationStart(const String& message);
    bool isNumericMessage(const String& message);
};

#endif
#endif

