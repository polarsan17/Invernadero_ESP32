#include "system/SystemManager.h"
#include "config/credentials.h"
#include "logic/LogicManager.h"

// Instancia estática para callbacks
SystemManager* SystemManager::instance = nullptr;

SystemManager::SystemManager(WiFiManager& wifi, BlynkManager& blynk) 
    : wifiManager(&wifi), blynkManager(&blynk), wifiConnected(false), blynkConnected(false), sensorsReady(false) {
    instance = this;
    sensorManager = new SensorManager(blynk);
    logicManager = new LogicManager();
    actuatorManager = new ActuatorManager(blynk);
}

SystemManager::~SystemManager() {
    if (sensorManager) {
        delete sensorManager;
    }
    if (logicManager) {
        delete logicManager;
    }
    if (actuatorManager) {
        delete actuatorManager;
    }
}

// cppcheck-suppress unusedFunction
bool SystemManager::initialize() {
    // Configurar callbacks WiFi
    wifiManager->onConnect(onWiFiConnectCallback);
    wifiManager->onDisconnect(onWiFiDisconnectCallback);
    
    // Configurar callbacks Blynk
    blynkManager->onConnect(onBlynkConnectCallback);
    blynkManager->onDisconnect(onBlynkDisconnectCallback);
    
    // Inicializar WiFiManager
    if (!wifiManager->begin()) {
        return false;
    }
    
    // Conectar a WiFi
    wifiManager->setCredentials(WIFI_SSID, WIFI_PASSWORD);
    if (!wifiManager->connectToWiFi()) {
        return false;
    }
    
    // Configurar Blynk
    blynkManager->begin(BLYNK_AUTH_TOKEN);
    
    // Inicializar sensores
    if (sensorManager->begin()) {
        sensorsReady = true;
        Serial.println("Sensores inicializados");
    } else {
        Serial.println("Error al inicializar sensores");
    }
    
    // Inicializar actuadores
    if (actuatorManager->begin()) {
        Serial.println("Actuadores inicializados");
    } else {
        Serial.println("Error al inicializar actuadores");
        return false;
    }
    
    // Inicializar LogicManager
    if (logicManager->begin(sensorManager, actuatorManager, blynkManager)) {
        Serial.println("LogicManager inicializado");
    } else {
        Serial.println("Error al inicializar LogicManager");
        return false;
    }
    
    return true;
}

void SystemManager::update() {
    // Gestionar reconexiones
    wifiManager->attemptReconnection();
    blynkManager->attemptReconnection();
    
    // Ejecutar Blynk si está conectado
    if (blynkConnected) {
        blynkManager->run();
    }
    
    // Actualizar sensores
    if (sensorsReady) {
        sensorManager->update();
    }
    
    // Actualizar actuadores
    if (actuatorManager) {
        actuatorManager->update();
    }
    
    // Actualizar lógica de control
    if (logicManager) {
        logicManager->update();
    }
}

// Callbacks estáticos
void SystemManager::onWiFiConnectCallback() {
    if (instance) instance->onWiFiConnect();
}

void SystemManager::onWiFiDisconnectCallback() {
    if (instance) instance->onWiFiDisconnect();
}

void SystemManager::onBlynkConnectCallback() {
    if (instance) instance->onBlynkConnect();
}

void SystemManager::onBlynkDisconnectCallback() {
    if (instance) instance->onBlynkDisconnect();
}

// Implementación de callbacks
void SystemManager::onWiFiConnect() {
    wifiConnected = true;
    
    // Intentar conectar a Blynk cuando WiFi esté conectado
    if (blynkManager->begin()) {
        blynkManager->connect();
    }
}

void SystemManager::onWiFiDisconnect() {
    wifiConnected = false;
    blynkConnected = false;
    
    // Desconectar Blynk cuando se pierda WiFi
    blynkManager->disconnect();
}

void SystemManager::onBlynkConnect() {
    blynkConnected = true;
}

void SystemManager::onBlynkDisconnect() {
    blynkConnected = false;
}
