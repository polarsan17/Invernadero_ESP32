#ifndef BLYNK_MANAGER_H
#define BLYNK_MANAGER_H

#include <WiFi.h>

class BlynkManager {
private:
    String authToken;
    String blynkServer;
    int blynkPort;
    
    bool isInitialized;
    unsigned long reconnectInterval;
    unsigned long lastConnectionAttempt;
    bool connectionStatus;
    
    // Punteros a funciones para acceder a Blynk sin incluir la librería
    bool (*blynkConnectFunc)();
    bool (*blynkConnectedFunc)();
    void (*blynkRunFunc)();
    void (*blynkConfigFunc)(const char*, const char*, int);
    void (*blynkVirtualWriteFunc)(int, float);
    void (*blynkVirtualWriteIntFunc)(int, int);
    void (*blynkVirtualWriteStringFunc)(int, String);
    
public:
    BlynkManager();
    ~BlynkManager();
    
    // Configuración inicial
    bool begin();
    bool begin(String token);
    bool begin(String token, String server, int port = 80);
    
    // Configurar funciones de acceso a Blynk (llamar desde main.cpp)
    void setBlynkFunctions(
        bool (*connectFunc)(),
        bool (*connectedFunc)(),
        void (*runFunc)(),
        void (*configFunc)(const char*, const char*, int),
        void (*virtualWriteFunc)(int, float),
        void (*virtualWriteIntFunc)(int, int),
        void (*virtualWriteStringFunc)(int, String)
    );
    
    // Gestión de conexión
    bool connect();
    bool isConnected();
    void disconnect();
    void run();
    
    // Configuración
    void setAuthToken(String token);
    void setServer(String server, int port = 80);
    void setReconnectInterval(unsigned long interval);
    
    // Utilidades
    bool attemptReconnection();
    String getAuthToken();
    
    // Envío de datos
    void sendVirtualPin(int pin, float value);
    void sendVirtualPin(int pin, int value);
    void sendVirtualPin(int pin, String value);
    
    // Callbacks
    void onConnect(void (*callback)());
    void onDisconnect(void (*callback)());
    
private:
    void (*connectCallback)();
    void (*disconnectCallback)();
};

#endif
