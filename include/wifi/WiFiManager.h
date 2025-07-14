#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>

class WiFiManager {
private:
    String ssid;
    String password;
    
    unsigned long connectionTimeout;
    unsigned long retryInterval;
    unsigned long lastConnectionAttempt;
    
public:
    WiFiManager();
    ~WiFiManager();
    
    // Métodos principales
    bool begin();
    bool connectToWiFi();
    bool connectToWiFi(String ssid, String password);
    
    // Configuración
    void setConnectionTimeout(unsigned long timeout);
    void setRetryInterval(unsigned long interval);
    void setCredentials(String ssid, String password);
    
    // Estado de conexión
    bool isConnected();
    String getSSID();
    String getIP();
    int getRSSI();
    
    // Utilidades
    void disconnect();
    bool attemptReconnection();
    
    // Callbacks
    void onConnect(void (*callback)());
    void onDisconnect(void (*callback)());
    
private:
    void (*connectCallback)();
    void (*disconnectCallback)();
};

#endif
