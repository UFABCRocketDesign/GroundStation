#include "UrdSimulation.h"

#if URD_SIMULATION_ENABLE


UrdSimulation::UrdSimulation(int baudRate)
    : UrdBase(baudRate)
{
    simulationStarted = false;

    pressure = 0.0f;
    height = 0.0f;
    initialHeight = 0.0f;
    initialPressure = 0.0f;
    maxHeight = 0.0f;
}

void UrdSimulation::startHandshake()
{
    appStarted = false;
    readyReceived = false;
    simulationStarted = false;

    Serial.println(F("Starting URD Simulation..."));

    while (!appStarted)
    {
        String message;

        if (readAppMessage(message))
        {
            if (processReady(message)) { continue; }
            if (processReset(message)) { continue; }
            if (processSimulationStart(message)) { continue; }

            processUnknownMessage(message);
        }

        delay(10);
    }

    ledOn();
}

bool UrdSimulation::processSimulationStart(const String& message)
{
    if (message != UrdProtocol::SIMULATION)
    {
        return false;
    }

    Serial.println(F("STARTED"));

    simulationStarted = true;
    markAppStarted();

    getInitialPressure();

    if (buzzerEnabled)
    {
        playDash();
    }

    return true;
}

bool UrdSimulation::update()
{
    return updateSimulation();
}

bool UrdSimulation::updateSimulation()
{
    if (Serial.available())
    {
        String message = Serial.readStringUntil('\n');
        message.trim();

        if (isNumericMessage(message))
        {
            pressure = message.toFloat();
            height = convertPressureToHeight(pressure);

            if (height > maxHeight)
            {
                maxHeight = height;
            }

            return true;
        }
    }

    return false;
}

bool UrdSimulation::isSimulationStarted() const
{
    return simulationStarted;
}

float UrdSimulation::getInitialPressure()
{
    float sum = 0.0f;
    int counter = 0;

    for (int i = 0; i < 10; i++)
    {
        while (!Serial.available())
        {
            delay(1);
        }

        float pressureValue = Serial.parseFloat();

        sum += pressureValue;
        counter++;
    }

    if (counter > 0)
    {
        initialPressure = sum / counter;
    }

    initialHeight = getInitialHeight(initialPressure);
    pressure = initialPressure;
    height = 0.0f;
    maxHeight = 0.0f;

    Serial.print(F("Simulator - Initial Pressure / Initial Height: "));
    Serial.print(initialPressure);
    Serial.print(F(" Pa \t"));
    Serial.print(initialHeight);
    Serial.println(F(" m"));

    return initialPressure;
}

float UrdSimulation::getInitialHeight()
{
    return initialHeight;
}

float UrdSimulation::getMaxHeight()
{
    return maxHeight;
}

float UrdSimulation::getPressure()
{
    return pressure;
}

float UrdSimulation::getHeight()
{
    return height;
}

float UrdSimulation::convertPressureToHeight(float pressureValue)
{
    height = 44330.0f *
             (1.0f - powf(pressureValue / 101325.0f, 1.0f / 5.25588f))
             - initialHeight;

    return height;
}

float UrdSimulation::getInitialHeight(float pressureValue)
{
    float absoluteHeight = 44330.0f *
                           (1.0f - powf(pressureValue / 101325.0f, 1.0f / 5.25588f));

    return absoluteHeight;
}

void UrdSimulation::restartInitialPressure(float pressureReference, float heightReference)
{
    initialPressure = pressureReference;
    initialHeight = getInitialHeight(initialPressure) - heightReference;

    pressure = pressureReference;
    height = 0.0f;
    maxHeight = 0.0f;

    Serial.print(F("Simulator restarted - Initial Pressure / Initial Height: "));
    Serial.print(initialPressure);
    Serial.print(F(" Pa \t"));
    Serial.print(initialHeight);
    Serial.println(F(" m"));
}

String UrdSimulation::getDataString()
{
    String data;

    data += String(pressure, 2);
    data += '\t';

    data += String(height, 2);
    data += '\t';

    return data;
}

String UrdSimulation::getHeaderString()
{
    String header;

    header += F("sim_press_pa\t");
    header += F("sim_height_m\t");

    return header;
}

bool UrdSimulation::isNumericMessage(const String& message)
{
    bool hasDigit = false;
    bool hasDot = false;

    for (int i = 0; i < message.length(); i++)
    {
        char character = message[i];

        if (character >= '0' && character <= '9')
        {
            hasDigit = true;
        }
        else if (character == '.' && !hasDot)
        {
            hasDot = true;
        }
        else if (character == '-' && i == 0)
        {
            continue;
        }
        else
        {
            return false;
        }
    }

    return hasDigit;
}

#endif
