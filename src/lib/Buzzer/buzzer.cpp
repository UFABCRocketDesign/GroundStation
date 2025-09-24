#include "buzzer.h"

BuzzerSistema::BuzzerSistema(int pin, int lPin, unsigned long intervalMillis, bool on, bool toneMode) : buzzerPin(pin), ledPin(lPin), interval(intervalMillis), buzzOn(on), toneMode(toneMode)
{
}

void BuzzerSistema::begin()
{
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  if (toneMode)
  {
    noTone(buzzerPin);
  }
  else
  {
    digitalWrite(buzzerPin, !buzzOn);
  }
}

void BuzzerSistema::beep(unsigned long currentTime)
{
  if (estadoBeep)
  {
    if (estadoBeepInterno)
    {
      if (toneMode)
      {
        tone(buzzerPin, 1000);
      }
      else
      {
        digitalWrite(buzzerPin, buzzOn);
      }
      digitalWrite(ledPin, HIGH);
    }
    else
    {
      if (toneMode)
      {
        noTone(buzzerPin);
      }
      else
      {
        digitalWrite(buzzerPin, !buzzOn);
      }
      digitalWrite(ledPin, LOW);
    }

    if (beepCount % 2 == 0)
    {
      beepDuration = 250;
    }
    else
    {
      if (beepPattern[readIndex] == false)
      {
        beepDuration = 600;
      }
      else
      {
        beepDuration = 200;
      }
    }

    if (currentTime - previousMillisInterno > beepDuration)
    {
      estadoBeepInterno = !estadoBeepInterno;
      previousMillisInterno = millis();
      if (beepCount % 2 != 0)
      {
        readIndex++;
      }
      if (++beepCount > maxBeeps * 2)
      {
        beepCount = 0;
        estadoBeep = !estadoBeep;
        estadoBeepInterno = false;
      }
    }
  }
  else
  {
    if (toneMode)
    {
      noTone(buzzerPin);
    }
    else
    {
      digitalWrite(buzzerPin, !buzzOn);
    }
    digitalWrite(ledPin, LOW);
  }
  if (currentTime - previousMillis > interval)
  {
#if DEBUG
    for (int i = 0; i < N_SISTEMAS; ++i)
    {
      Serial.print(beepPattern[i] ? '.' : '-');
    }
    Serial.println(); 
#endif // DEBUG
    estadoBeep = !estadoBeep;
    previousMillis = millis();
    previousMillisInterno = previousMillis;
    readIndex = 0;
  }
}

void BuzzerSistema::addSystem(bool booleano)
{
  beepPattern[writeIndex] = booleano;

  writeIndex = (writeIndex + 1) % maxBeeps;
}

//--------------------------------

BuzzerEvento::BuzzerEvento(int pino, bool on, unsigned long intervalo, unsigned long duracao, bool toneMode)
  : pin(pino), pinOn(on), interval(intervalo), onDuration(duracao), beepOnTone(toneMode) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, !pinOn);
}

void BuzzerEvento::loop(unsigned long time) {
  seal();
  if (pinSeal) {
    if (timesCount < maxTimes) {
      if (time <= previousMillis + onDuration) {
        if (!beepOnTone) {
          digitalWrite(pin, pinOn);
        } else {
          tone(pin, 1000);
        }
      } else if (time > previousMillis + onDuration && time <= previousMillis + onDuration + interval) {
        if (!beepOnTone) {
          digitalWrite(pin, !pinOn);
        } else {
          noTone(pin);
        }
      } else {
        previousMillis = time;
        timesCount += 1;
      }
    } else {
      timesCount = 0;
      maxTimes = 0;
      unseal();
    }
  }
}

void BuzzerEvento::add() {
  if (pinSeal == false) {
    maxTimes += 1;
  }
}

void BuzzerEvento::seal() {
  if (!pinSeal && maxTimes != 0) {
    previousMillis = millis();
    pinSeal = true;
  }
}

void BuzzerEvento::unseal() {
  pinSeal = false;
}