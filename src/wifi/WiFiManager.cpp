#include "wifi/WiFiManager.h"
#include "config/config.h"

WiFiManager::WiFiManager() {
    connectionTimeout = WIFI_CONNECTION_TIMEOUT;
    retryInterval = WIFI_RETRY_INTERVAL;
    lastConnectionAttempt = 0;
    connectCallback = nullptr;
    disconnectCallback = nullptr;
}

WiFiManager::~WiFiManager() {
    // Cleanup if needed
}

bool WiFiManager::begin() {
    // Configurar WiFi en modo estación
    WiFi.mode(WIFI_STA);
    return true;
}

bool WiFiManager::connectToWiFi() {
    if (ssid.length() == 0) {
        return false;
    }
    
    return connectToWiFi(ssid, password);
}

bool WiFiManager::connectToWiFi(String newSSID, String newPassword) {
    // Guardar credenciales
    ssid = newSSID;
    password = newPassword;
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < connectionTimeout) {
        delay(500);
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        if (connectCallback) {
            connectCallback();
        }
        return true;
    } else {
        return false;
    }
}

// cppcheck-suppress unusedFunction
bool WiFiManager::attemptReconnection() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastConnectionAttempt >= retryInterval) {
        lastConnectionAttempt = currentTime;
        
        if (!isConnected() && ssid.length() > 0) {
            return connectToWiFi();
        }
    }
    
    return isConnected();
}

bool WiFiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

// cppcheck-suppress unusedFunction
String WiFiManager::getSSID() {
    return WiFi.SSID();
}

// cppcheck-suppress unusedFunction
String WiFiManager::getIP() {
    return WiFi.localIP().toString();
}

// cppcheck-suppress unusedFunction
int WiFiManager::getRSSI() {
    return WiFi.RSSI();
}

// cppcheck-suppress unusedFunction
void WiFiManager::setConnectionTimeout(unsigned long timeout) {
    connectionTimeout = timeout;
}

// cppcheck-suppress unusedFunction
void WiFiManager::setRetryInterval(unsigned long interval) {
    retryInterval = interval;
}

// cppcheck-suppress unusedFunction
void WiFiManager::setCredentials(String newSSID, String newPassword) {
    ssid = newSSID;
    password = newPassword;
}

void WiFiManager::disconnect() {
    WiFi.disconnect();
    if (disconnectCallback) {
        disconnectCallback();
    }
}

// cppcheck-suppress unusedFunction  
void WiFiManager::onConnect(void (*callback)()) {
    connectCallback = callback;
}

// cppcheck-suppress unusedFunction
void WiFiManager::onDisconnect(void (*callback)()) {
    disconnectCallback = callback;
}
