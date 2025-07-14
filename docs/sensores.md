# Sistema de Sensores - Invernadero ESP32

## Descripción General

El sistema de monitoreo del invernadero integra seis sensores especializados para el control ambiental y análisis del suelo. Cada sensor proporciona datos específicos que permiten un monitoreo completo de las condiciones de crecimiento de las plantas.

## Lista de Sensores Integrados

### 1. Sensor DHT22 - Monitoreo Ambiental

**Valores que capta:**
- Temperatura del aire (°C)
- Humedad relativa (%)
- Índice de calor calculado (°C)

**Para qué sirve:**
- Control del clima interno del invernadero
- Activación automática de ventilación y calefacción
- Prevención de estrés térmico en las plantas

**Por qué es necesario en un invernadero:**
La temperatura y humedad son factores críticos para el crecimiento óptimo de las plantas. Un control inadecuado puede causar enfermedades fúngicas, estrés hídrico o reducir significativamente el rendimiento de los cultivos.

### 2. Sensor AS7341 - Análisis Espectral de Luz

**Valores que capta:**
- Intensidad luminosa total (lux)
- Temperatura de color (Kelvin)
- Espectro rojo, verde, azul
- Luz clara e infrarroja cercana (NIR)

**Para qué sirve:**
- Optimización de iluminación artificial LED
- Monitoreo de calidad de luz solar
- Control de fotoperiodo para floración

**Por qué es necesario en un invernadero:**
La luz es esencial para la fotosíntesis. Diferentes espectros afectan el crecimiento vegetativo, floración y calidad de frutos. El análisis espectral permite maximizar la eficiencia fotosintética y reducir costos energéticos.

### 3. Sensor de Humedad del Suelo Capacitivo

**Valores que capta:**
- Humedad del sustrato (%)
- Valor analógico bruto
- Clasificación de humedad (seco/húmedo/saturado)

**Para qué sirve:**
- Programación automática de riego
- Prevención de sobre-riego o sequía
- Optimización del uso del agua

**Por qué es necesario en un invernadero:**
El riego inadecuado es la principal causa de pérdida de cultivos. Este sensor permite mantener la humedad óptima del sustrato, evitando la pudrición de raíces por exceso de agua o el estrés hídrico por sequía.

### 4. Sensor BH1750 - Luxómetro Digital

**Valores que capta:**
- Intensidad lumínica (lux)
- Clasificación de niveles de luz
- Medición de luz ambiente general

**Para qué sirve:**
- Control de iluminación suplementaria
- Monitoreo de condiciones lumínicas naturales
- Activación automática de luces LED

**Por qué es necesario en un invernadero:**
Complementa el análisis espectral proporcionando mediciones precisas de intensidad lumínica total. Es fundamental para determinar cuándo activar la iluminación artificial y asegurar que las plantas reciban la cantidad mínima de luz requerida.

### 5. Sensor HC-SR04 - Monitor de Nivel de Agua

**Valores que capta:**
- Distancia al agua (cm)
- Nivel de agua en reservorio
- Porcentaje de capacidad del tanque
- Estado del agua (lleno/medio/bajo/vacío)

**Para qué sirve:**
- Monitoreo del suministro de agua
- Alertas de reabastecimiento necesario
- Prevención de fallas en el sistema de riego

**Por qué es necesario en un invernadero:**
Un suministro continuo de agua es vital para el funcionamiento del sistema de riego. Este sensor previene interrupciones en el riego por falta de agua y permite planificar el reabastecimiento del reservorio.

### 6. Sensor RS485 de Suelo Industrial - Análisis Completo del Suelo

**Valores que capta:**
- Temperatura del suelo (°C)
- Humedad del sustrato (%)
- Conductividad eléctrica - EC (µS/cm)
- pH del suelo
- Nitrógeno, fósforo, potasio - NPK (mg/kg)

**Para qué sirve:**
- Análisis profesional de fertilidad del suelo
- Control de salinidad y pH
- Gestión precisa de fertilización
- Monitoreo de disponibilidad de nutrientes

**Por qué es necesario en un invernadero:**
Proporciona un análisis completo de la química del suelo, permitiendo ajustes precisos en fertilización y pH. La medición de NPK evita deficiencias nutricionales y optimiza el crecimiento, mientras que el control de EC previene la salinización del sustrato.

## Integración del Sistema

Todos los sensores están integrados en un sistema unificado que:

- Realiza lecturas periódicas automáticas
- Valida la integridad de los datos
- Transmite información en tiempo real a la plataforma Blynk
- Proporciona diagnósticos del estado de cada sensor
- Mantiene un registro continuo para análisis histórico

## Monitoreo y Control

El sistema permite:

- Visualización en tiempo real de todos los parámetros
- Configuración de intervalos de lectura personalizados
- Alertas automáticas por valores fuera de rango
- Análisis de tendencias para optimización del cultivo
- Control remoto a través de la aplicación móvil Blynk

Este conjunto de sensores proporciona una solución completa para el monitoreo profesional de invernaderos, cubriendo aspectos ambientales, lumínicos, hídricos y edáficos necesarios para un cultivo óptimo.
