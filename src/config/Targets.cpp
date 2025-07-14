#include "config/Targets.h"

Targets targets;

void Targets::loadDefaults() {
    temperature = 24.0;
    humidity = 60.0;
    soilMoisture = 40.0;
    luxMin = 10000.0;
    ventTemp = 28.0;
    soilPH = 6.5;
    soilEC = 1500.0;
    waterLevel = 20.0;
}
