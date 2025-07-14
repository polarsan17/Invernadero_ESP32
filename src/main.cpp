
#include <Arduino.h>
#include "config/config.h"
#include "config/Targets.h"
#include "system/SystemManager.h"
#include "wifi/WiFiManager.h"
#include "blynk/BlynkManager.h"
#include <BlynkSimpleEsp32.h>


WiFiManager wifiManager;
BlynkManager blynkManager;
SystemManager systemManager(wifiManager, blynkManager);

// ...existing code...


BLYNK_WRITE(V40) {  // Target Temperatura
    targets.temperature = param.asFloat();
    Serial.println(String("[Blynk] Nueva temperatura objetivo: ") + targets.temperature + "°C");
    Blynk.virtualWrite(BLYNK_VPIN_TARGET_TEMPERATURE, targets.temperature);
}
BLYNK_WRITE(V41) {  // Target Humedad
    targets.humidity = param.asFloat();
    Serial.println(String("[Blynk] Nueva humedad objetivo: ") + targets.humidity + "%");
    Blynk.virtualWrite(BLYNK_VPIN_TARGET_HUMIDITY, targets.humidity);
}
BLYNK_WRITE(V42) {  // Target Humedad del Suelo
    targets.soilMoisture = param.asFloat();
    Serial.println(String("[Blynk] Nueva humedad suelo objetivo: ") + targets.soilMoisture + "%");
    Blynk.virtualWrite(BLYNK_VPIN_TARGET_SOIL_MOISTURE, targets.soilMoisture);
}
BLYNK_WRITE(V43) {  // Target Lux Mínimo
    targets.luxMin = param.asFloat();
    Serial.println(String("[Blynk] Nuevo lux mínimo: ") + targets.luxMin + " lux");
    Blynk.virtualWrite(BLYNK_VPIN_TARGET_LUX_MIN, targets.luxMin);
}
BLYNK_WRITE(V44) {  // Target Temperatura Ventilación
    targets.ventTemp = param.asFloat();
    Serial.println(String("[Blynk] Nueva temperatura ventilación: ") + targets.ventTemp + "°C");
    Blynk.virtualWrite(BLYNK_VPIN_TARGET_VENT_TEMP, targets.ventTemp);
}
BLYNK_WRITE(V45) {  // Target pH del Suelo
    targets.soilPH = param.asFloat();
    Serial.println(String("[Blynk] Nuevo pH suelo objetivo: ") + targets.soilPH);
    Blynk.virtualWrite(BLYNK_VPIN_TARGET_SOIL_PH, targets.soilPH);
}
BLYNK_WRITE(V46) {  // Target EC del Suelo
    targets.soilEC = param.asFloat();
    Serial.println(String("[Blynk] Nueva EC suelo objetivo: ") + targets.soilEC + " µS/cm");
    Blynk.virtualWrite(BLYNK_VPIN_TARGET_SOIL_EC, targets.soilEC);
}
BLYNK_WRITE(V47) {  // Target Nivel de Agua Crítico
    targets.waterLevel = param.asFloat();
    Serial.println(String("[Blynk] Nuevo nivel agua crítico: ") + targets.waterLevel + "%");
    Blynk.virtualWrite(BLYNK_VPIN_TARGET_WATER_LEVEL, targets.waterLevel);
}


BLYNK_CONNECTED() {
    Serial.println("[Blynk] Conectado - Sincronizando targets...");
    Blynk.syncVirtual(BLYNK_VPIN_TARGET_TEMPERATURE);
    Blynk.syncVirtual(BLYNK_VPIN_TARGET_HUMIDITY);
    Blynk.syncVirtual(BLYNK_VPIN_TARGET_SOIL_MOISTURE);
    Blynk.syncVirtual(BLYNK_VPIN_TARGET_LUX_MIN);
    Blynk.syncVirtual(BLYNK_VPIN_TARGET_VENT_TEMP);
    Blynk.syncVirtual(BLYNK_VPIN_TARGET_SOIL_PH);
    Blynk.syncVirtual(BLYNK_VPIN_TARGET_SOIL_EC);
    Blynk.syncVirtual(BLYNK_VPIN_TARGET_WATER_LEVEL);
    // Enviar valores actuales a Blynk
    Blynk.virtualWrite(BLYNK_VPIN_TARGET_TEMPERATURE, targets.temperature);
    Blynk.virtualWrite(BLYNK_VPIN_TARGET_HUMIDITY, targets.humidity);
    Blynk.virtualWrite(BLYNK_VPIN_TARGET_SOIL_MOISTURE, targets.soilMoisture);
    Blynk.virtualWrite(BLYNK_VPIN_TARGET_LUX_MIN, targets.luxMin);
    Blynk.virtualWrite(BLYNK_VPIN_TARGET_VENT_TEMP, targets.ventTemp);
    Blynk.virtualWrite(BLYNK_VPIN_TARGET_SOIL_PH, targets.soilPH);
    Blynk.virtualWrite(BLYNK_VPIN_TARGET_SOIL_EC, targets.soilEC);
    Blynk.virtualWrite(BLYNK_VPIN_TARGET_WATER_LEVEL, targets.waterLevel);
    Serial.println("[Blynk] Sincronización de targets completada");
}

void setup() {
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println("=== ESP32 Invernadero con Targets Ajustables ===");
    targets.loadDefaults();
    if (systemManager.initialize()) {
        Serial.println("Sistema iniciado correctamente");
    } else {
        Serial.println("Error al inicializar sistema");
    }
}


void loop() {
    systemManager.update();
    // Aquí puedes usar targets.temperature, targets.humidity, etc. en tu lógica de control
    delay(100);
}
