#include "blynk/blynk_config.h"
#include "blynk/BlynkManager.h"
#include "config/config.h"

BlynkManager::BlynkManager() 
    : authToken(""),
      blynkServer("blynk.cloud"),
      blynkPort(80),
      isInitialized(false),
      lastConnectionAttempt(0),
      reconnectInterval(BLYNK_RECONNECT_INTERVAL),
      connectCallback(nullptr),
      disconnectCallback(nullptr),
      connectionStatus(false),
      blynkRunFunc(nullptr),
      blynkConnectedFunc(nullptr),
      blynkVirtualWriteFunc(nullptr),
      blynkConnectFunc(nullptr),
      blynkConfigFunc(nullptr) {
    // Constructor completado con lista de inicialización
    blynkVirtualWriteIntFunc = nullptr;
    blynkVirtualWriteStringFunc = nullptr;
}

BlynkManager::~BlynkManager() {
    // Cleanup if needed
}

bool BlynkManager::begin() {
    if (authToken.length() == 0) {
        return false;
    }
    
    isInitialized = true;
    return true;
}

bool BlynkManager::begin(String token) {
    authToken = token;
    blynkServer = "blynk.cloud";
    blynkPort = 80;
    
    isInitialized = true;
    return true;
}

bool BlynkManager::begin(String token, String server, int port) {
    authToken = token;
    blynkServer = server;
    blynkPort = port;
    
    isInitialized = true;
    return true;
}

// cppcheck-suppress unusedFunction
void BlynkManager::setBlynkFunctions(
    bool (*connectFunc)(),
    bool (*connectedFunc)(),
    void (*runFunc)(),
    void (*configFunc)(const char*, const char*, int),
    void (*virtualWriteFunc)(int, float),
    void (*virtualWriteIntFunc)(int, int),
    void (*virtualWriteStringFunc)(int, String)
) {
    blynkConnectFunc = connectFunc;
    blynkConnectedFunc = connectedFunc;
    blynkRunFunc = runFunc;
    blynkConfigFunc = configFunc;
    blynkVirtualWriteFunc = virtualWriteFunc;
    blynkVirtualWriteIntFunc = virtualWriteIntFunc;
    blynkVirtualWriteStringFunc = virtualWriteStringFunc;
}

bool BlynkManager::connect() {
    if (!isInitialized || !blynkConfigFunc || !blynkConnectedFunc || !blynkRunFunc) {
        return false;
    }
    
    if (WiFi.status() != WL_CONNECTED) {
        return false;
    }
    
    if (authToken.length() == 0) {
        return false;
    }
    
    // Configurar Blynk
    blynkConfigFunc(authToken.c_str(), blynkServer.c_str(), blynkPort);
    
    // Intentar conexión
    unsigned long startTime = millis();
    while (!blynkConnectedFunc() && millis() - startTime < 10000) { // 10 segundos timeout
        blynkRunFunc();
        delay(100);
    }
    
    connectionStatus = blynkConnectedFunc();
    if (connectionStatus && connectCallback) {
        connectCallback();
    }
    
    return connectionStatus;
}

bool BlynkManager::isConnected() {
    if (blynkConnectedFunc) {
        connectionStatus = blynkConnectedFunc();
    }
    return connectionStatus;
}

// cppcheck-suppress unusedFunction
void BlynkManager::disconnect() {
    if (disconnectCallback) {
        disconnectCallback();
    }
}

// cppcheck-suppress unusedFunction
void BlynkManager::run() {
    if (isInitialized && blynkRunFunc) {
        blynkRunFunc();
    }
}

// cppcheck-suppress unusedFunction
bool BlynkManager::attemptReconnection() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastConnectionAttempt >= reconnectInterval) {
        lastConnectionAttempt = currentTime;
        
        if (!isConnected() && WiFi.status() == WL_CONNECTED) {
            return connect();
        }
    }
    
    return isConnected();
}

// cppcheck-suppress unusedFunction
void BlynkManager::setAuthToken(String token) {
    authToken = token;
}

// cppcheck-suppress unusedFunction
void BlynkManager::setServer(String server, int port) {
    blynkServer = server;
    blynkPort = port;
}

// cppcheck-suppress unusedFunction
void BlynkManager::setReconnectInterval(unsigned long interval) {
    reconnectInterval = interval;
}

// cppcheck-suppress unusedFunction
String BlynkManager::getAuthToken() {
    return authToken;
}

// cppcheck-suppress unusedFunction
void BlynkManager::onConnect(void (*callback)()) {
    connectCallback = callback;
}

// cppcheck-suppress unusedFunction
void BlynkManager::onDisconnect(void (*callback)()) {
    disconnectCallback = callback;
}

// cppcheck-suppress unusedFunction
void BlynkManager::sendVirtualPin(int pin, float value) {
    if (isConnected() && blynkVirtualWriteFunc) {
        blynkVirtualWriteFunc(pin, value);
    }
}

void BlynkManager::sendVirtualPin(int pin, int value) {
    if (isConnected() && blynkVirtualWriteIntFunc) {
        blynkVirtualWriteIntFunc(pin, value);
    }
}

void BlynkManager::sendVirtualPin(int pin, String value) {
    if (isConnected() && blynkVirtualWriteStringFunc) {
        blynkVirtualWriteStringFunc(pin, value);
    }
}
