#pragma once

#include <Arduino.h>

struct Targets {
    float temperature = 24.0;
    float humidity = 60.0;
    float soilMoisture = 40.0;
    float luxMin = 10000.0;
    float ventTemp = 28.0;
    float soilPH = 6.5;
    float soilEC = 1500.0;
    float waterLevel = 20.0;

    void loadDefaults();
};

extern Targets targets;
