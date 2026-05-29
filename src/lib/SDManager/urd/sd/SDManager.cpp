#include "SDManager.h"

#if SD_CARD

SDManager::SDManager(
    int csPinConfig,
    int sckPinConfig,
    int misoPinConfig,
    int mosiPinConfig
)
#if defined(ARDUINO_ARCH_ESP32)
    #if CONFIG_IDF_TARGET_ESP32
        : spiBus(VSPI)
    #else
        : spiBus(FSPI)
    #endif
#endif
{
    csPin = csPinConfig;
    sckPin = sckPinConfig;
    misoPin = misoPinConfig;
    mosiPin = mosiPinConfig;

    started = false;
    printEnabled = true;
    debugEnabled = false;

    nameSD[0] = '\0';

    lastSaveTimeUs = 0;
}

void SDManager::setPrint(bool enable)
{
    printEnabled = enable;
}

void SDManager::setDebug(bool enable)
{
    debugEnabled = enable;
}

bool SDManager::begin(const String& baseName)
{
    started = false;
    nameSD[0] = '\0';

    if (!beginSdCard())
    {
        if (printEnabled)
        {
            Serial.println(F("SD err"));
        }

        return false;
    }

    if (printEnabled)
    {
        Serial.println(F("SD ok"));
    }

    createSdFileName(baseName);

    if (strcmp(nameSD, "error") == 0)
    {
        if (printEnabled)
        {
            Serial.println(F("SD filename err"));
        }

        return false;
    }

    started = true;

    if (printEnabled)
    {
        Serial.print(nameSD);
        Serial.println(F(" created."));
    }

    return true;
}

bool SDManager::beginSdCard()
{
    if (csPin < 0)
    {
        if (debugEnabled)
        {
            Serial.println(F("[SD DEBUG] CS pin invalido."));
        }

        return false;
    }

#if defined(ARDUINO_ARCH_ESP32)

    if (sckPin >= 0 && misoPin >= 0 && mosiPin >= 0)
    {
        spiBus.begin(sckPin, misoPin, mosiPin, csPin);
        return SD.begin(csPin, spiBus);
    }

    return SD.begin(csPin);

#else

    return SD.begin(csPin);

#endif
}

void SDManager::createSdFileName(const String& baseName)
{
    int fileIndex = 0;
    unsigned long startTime = millis();

    while (true)
    {
        if (millis() - startTime > 10000UL)
        {
            strcpy(nameSD, "error");
            if (debugEnabled)
            {
                Serial.println(F("[SD DEBUG] fail to create filename, timeout exceeded."));
            }
            break;
        }

        int numZeros = 8 - baseName.length() - String(fileIndex).length();

        String zeros = "";

        for (int i = 0; i < numZeros - 1; i++)
        {
            zeros += "0";
        }

#if defined(ARDUINO_ARCH_ESP32)
        String fullName = "/";
#else
        String fullName = "";
#endif

        fullName += baseName;
        fullName += zeros;
        fullName += String(fileIndex);
        fullName += ".txt";

        fullName.toCharArray(nameSD, sizeof(nameSD));

        if (SD.exists(nameSD))
        {
            fileIndex++;
            if (debugEnabled)
            {
                Serial.print(F("[SD DEBUG] filename "));
                Serial.print(nameSD);
                Serial.print(F("exists, trying next index: "));
                Serial.println(fileIndex);
            }
        }
        else
        {
            File testFile = openForWrite(nameSD);

            if (testFile)
            {
                testFile.close();
            }
            else
            {
                strcpy(nameSD, "error");
            }

            break;
        }
    delay(1);
    }
}

bool SDManager::write(const String& data)
{
    return openAndWrite(data, false);
}

bool SDManager::writeLine(const String& data)
{
    return openAndWrite(data, true);
}

bool SDManager::openAndWrite(const String& data, bool newLine)
{
    if (!started)
    {
        if (debugEnabled)
        {
            Serial.println(F("[SD DEBUG] attempt to write with SD not initialized."));
        }

        return false;
    }

    unsigned long startUs = micros();

    File dataFile = openForWrite(nameSD);

    if (!dataFile)
    {
        if (debugEnabled)
        {
            Serial.print(F("[SD DEBUG] fail to open file for writing: "));
            Serial.println(nameSD);
        }

        return false;
    }

    if (newLine)
    {
        dataFile.println(data);
    }
    else
    {
        dataFile.print(data);
    }

    dataFile.flush();
    dataFile.close();

    lastSaveTimeUs = micros() - startUs;

    return true;
}

bool SDManager::read(Print& output, const char* path, unsigned long maxReadMs)
{
    if (!started)
    {
        if (debugEnabled)
        {
            Serial.println(F("[SD DEBUG] attempt to read with SD not initialized."));
        }

        return false;
    }

    String realPath;

    if (path == nullptr)
    {
        realPath = String(nameSD);
    }
    else
    {
        realPath = normalizePath(path);
    }

    File dataFile = SD.open(realPath.c_str(), FILE_READ);

    if (!dataFile)
    {
        if (debugEnabled)
        {
            Serial.print(F("[SD DEBUG] fail to open file for reading: "));
            Serial.println(realPath);
        }

        return false;
    }

    unsigned long startTime = millis();

    while (dataFile.available())
    {
        if (millis() - startTime > maxReadMs)
        {
            if (debugEnabled)
            {
                Serial.println(F("[SD DEBUG] read operation interrupted by timeout."));
            }

            break;
        }

        output.write(dataFile.read());
        delay(1);
    }

    dataFile.close();

    return true;
}

bool SDManager::listFiles(Print& output, const char* dirname, uint8_t levels)
{
    if (!started)
    {
        if (debugEnabled)
        {
            Serial.println(F("[SD DEBUG] attempt to list files with SD not initialized."));
        }

        return false;
    }

    String realPath = normalizePath(dirname);

    File root = SD.open(realPath.c_str());

    if (!root)
    {
        if (debugEnabled)
        {
            Serial.print(F("[SD DEBUG] fail to open directory: "));
            Serial.println(realPath);
        }

        return false;
    }

    if (!root.isDirectory())
    {
        if (debugEnabled)
        {
            Serial.println(F("[SD DEBUG] path is not a directory."));
        }

        root.close();
        return false;
    }

    listDirectory(root, output, levels);

    root.close();

    return true;
}

void SDManager::listDirectory(File dir, Print& output, uint8_t levels)
{
    while (true)
    {
        File entry = dir.openNextFile();

        if (!entry)
        {
            break;
        }

        output.print(entry.name());

        if (entry.isDirectory())
        {
            output.println(F("/"));

            if (levels > 0)
            {
                listDirectory(entry, output, levels - 1);
            }
        }
        else
        {
            output.print(F("\t"));
            output.print(entry.size());
            output.println(F(" bytes"));
        }

        entry.close();
        delay(1);
    }
}

String SDManager::normalizePath(const char* path) const
{
    if (path == nullptr || path[0] == '\0')
    {
#if defined(ARDUINO_ARCH_ESP32)
        return String("/");
#else
        return String("");
#endif
    }

    String fixedPath = String(path);

#if defined(ARDUINO_ARCH_ESP32)

    if (fixedPath[0] != '/')
    {
        fixedPath = "/" + fixedPath;
    }

#else

    if (fixedPath[0] == '/')
    {
        fixedPath.remove(0, 1);
    }

#endif

    return fixedPath;
}

File SDManager::openForWrite(const char* path)
{
#if defined(ARDUINO_ARCH_ESP32)
    return SD.open(path, FILE_APPEND);
#else
    return SD.open(path, FILE_WRITE);
#endif
}

bool SDManager::isReady() const
{
    return started;
}

const char* SDManager::getFileName() const
{
    return nameSD;
}

unsigned long SDManager::getLastSaveTimeUs() const
{
    return lastSaveTimeUs;
}

#endif