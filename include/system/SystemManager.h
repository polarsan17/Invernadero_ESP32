#pragma once

#include "wifi/WiFiManager.h"
#include "blynk/BlynkManager.h"
#include "sensors/SensorManager.h"
#include "logic/LogicManager.h"
#include "actuators/ActuatorManager.h"

class SystemManager {
private:
    WiFiManager* wifiManager;
    BlynkManager* blynkManager;
    SensorManager* sensorManager;
    LogicManager* logicManager;
    ActuatorManager* actuatorManager;
    // Variables de estado
    bool wifiConnected;
    bool blynkConnected;
    bool sensorsReady;
    // Callbacks internos
    static void onWiFiConnectCallback();
    static void onWiFiDisconnectCallback();
    static void onBlynkConnectCallback();
    static void onBlynkDisconnectCallback();
    static SystemManager* instance;
    
public:
    SystemManager(WiFiManager& wifi, BlynkManager& blynk);
    ~SystemManager();
    
    // Deshabilitar copia y asignación para manejo seguro de memoria dinámica
    SystemManager(const SystemManager&) = delete;
    SystemManager& operator=(const SystemManager&) = delete;
    
    bool initialize();
    void update();
    
    // Callbacks públicos
    void onWiFiConnect();
    void onWiFiDisconnect();
    void onBlynkConnect();
    void onBlynkDisconnect();
    
    // Getters de estado
    bool isWiFiConnected() const { return wifiConnected; }
    bool isBlynkConnected() const { return blynkConnected; }
    bool areSensorsReady() const { return sensorsReady; }
    
    // Acceso a managers
    SensorManager* getSensorManager() { return sensorManager; }
};
