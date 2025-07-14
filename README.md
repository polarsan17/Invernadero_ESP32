
# Sistema de Monitoreo y Control de Invernadero ESP32

<p align="center">
  <img src="docs/invernadero_banner.png" alt="Invernadero ESP32" width="60%">
</p>

---

## Tabla de Contenidos

- [Descripción General](#descripción-general)
- [Arquitectura del Sistema](#arquitectura-del-sistema)
- [Características Técnicas](#características-técnicas)
- [Beneficios y Aplicaciones](#beneficios-y-aplicaciones)
- [Estructura del Proyecto](#estructura-del-proyecto)
- [Requisitos del Sistema](#requisitos-del-sistema)
- [Instalación y Compilación](#instalación-y-compilación)
- [Ejemplo de Uso](#ejemplo-de-uso)
- [Documentación Técnica](#documentación-técnica)
- [Contribución y Desarrollo](#contribución-y-desarrollo)
- [Licenciamiento](#licenciamiento)



## Descripción General

El Sistema de Monitoreo y Control de Invernadero ESP32 es una solución integral de automatización agrícola diseñada para optimizar las condiciones de cultivo mediante el monitoreo continuo de parámetros ambientales y del suelo, junto con el control automatizado de sistemas de riego, ventilación e iluminación.

## Arquitectura del Sistema

### Componentes Principales

El sistema está construido sobre una arquitectura modular que integra múltiples tecnologías:

- **Microcontrolador ESP32**: Unidad central de procesamiento con capacidades WiFi integradas
- **Red de Sensores**: Conjunto de sensores especializados para monitoreo ambiental y del suelo
- **Sistema de Actuadores**: Relés y controladores para bombas, ventiladores, válvulas e iluminación
- **Interfaz de Usuario**: Aplicación móvil Blynk para control remoto y monitoreo
- **Comunicación Inalámbrica**: Conectividad WiFi para transmisión de datos en tiempo real

### Capacidades de Monitoreo

#### Parámetros Ambientales
- **Temperatura y Humedad del Aire**: Sensor DHT22 para control climático preciso
- **Luminosidad Ambiente**: Sensor BH1750 para medición de intensidad lumínica
- **Análisis Espectral**: Sensor AS7341 para evaluación de calidad de luz
- **Nivel de Agua**: Sensor ultrasónico HC-SR04 para monitoreo de reservorios

#### Parámetros del Suelo
- **Humedad del Sustrato**: Sensor capacitivo resistente a la corrosión
- **Análisis Químico del Suelo**: Sensor RS485 con protocolo Modbus para medición de:
  - Temperatura del suelo
  - Humedad volumétrica
  - Nivel de pH
  - Conductividad eléctrica (EC)
  - Nutrientes NPK (Nitrógeno, Fósforo, Potasio)

### Sistemas de Control Automatizado

#### Gestión Hídrica
- **Riego Automatizado**: Sistema de bombas con control por zonas
- **Monitoreo de Niveles**: Alertas automáticas para recarga de reservorios
- **Distribución de Nutrientes**: Bomba especializada para soluciones nutritivas

#### Control Climático
- **Ventilación Inteligente**: Ventiladores de extracción y circulación
- **Regulación Térmica**: Sistema de calefacción con control de temperatura
- **Gestión de Humedad**: Control automático basado en umbrales configurables

#### Iluminación Optimizada
- **LEDs de Espectro Completo**: Iluminación artificial para períodos de baja luminosidad
- **Control Automático**: Activación basada en mediciones de luz natural
- **Programación Horaria**: Ciclos de iluminación personalizables

## Características Técnicas

### Conectividad y Comunicación
- **WiFi 802.11 b/g/n**: Conectividad inalámbrica estable
- **Protocolo Blynk**: Interfaz de usuario intuitiva para dispositivos móviles
- **Comunicación RS485**: Protocolo Modbus RTU para sensores industriales
- **I2C Multi-dispositivo**: Bus de comunicación para sensores digitales

### Gestión de Datos
- **Almacenamiento Local**: Memoria EEPROM para configuraciones persistentes
- **Transmisión en Tiempo Real**: Envío continuo de datos a la plataforma Blynk
- **Configuración Remota**: Ajuste de parámetros desde la aplicación móvil
- **Histórico de Datos**: Gráficos y tendencias en la interfaz de usuario

### Seguridad y Confiabilidad
- **Protección Eléctrica**: Fusibles y protecciones contra sobrecargas
- **Validación de Datos**: Verificación de rangos válidos para todas las lecturas
- **Reconexión Automática**: Recuperación automática de conexiones perdidas
- **Watchdog Timer**: Reinicio automático en caso de fallos del sistema

## Beneficios y Aplicaciones

### Optimización Agrícola
- **Condiciones Óptimas**: Mantenimiento automático de parámetros ideales de cultivo
- **Eficiencia Hídrica**: Riego preciso basado en necesidades reales de las plantas
- **Calidad del Suelo**: Monitoreo continuo de nutrientes y pH
- **Ahorro Energético**: Automatización inteligente de sistemas de soporte

### Monitoreo Remoto
- **Acceso Ubicuo**: Control desde cualquier ubicación con conexión a internet
- **Alertas Instantáneas**: Notificaciones automáticas de condiciones críticas
- **Histórico Detallado**: Análisis de tendencias y patrones de cultivo
- **Configuración Flexible**: Ajuste de umbrales y parámetros operativos

### Aplicaciones Específicas
- **Invernaderos Comerciales**: Producción a gran escala con control automatizado
- **Agricultura Urbana**: Sistemas de cultivo en espacios reducidos
- **Investigación Agrícola**: Monitoreo preciso para estudios científicos
- **Cultivo Hidropónico**: Control especializado para sistemas sin suelo


## Instalación y Compilación

### 1. Clonar el repositorio
```bash
git clone https://github.com/tuusuario/Invernadero_ESP32.git
cd Invernadero_ESP32
```

### 2. Instalar dependencias
- Instala [PlatformIO](https://platformio.org/) en VS Code.
- Abre la carpeta del proyecto en VS Code.
- PlatformIO instalará automáticamente las dependencias al compilar.

### 3. Compilar y cargar el firmware
Selecciona la placa ESP32 adecuada y ejecuta:
```bash
pio run --target upload
```

### 4. Configurar credenciales WiFi y Blynk
Edita `include/config/credentials.h` con tu token de Blynk y datos WiFi.

---

## Ejemplo de Uso

1. Ajusta los targets de temperatura, humedad, luz y riego desde la app Blynk.
2. El sistema monitorea y controla automáticamente el ambiente del invernadero.
3. Visualiza datos históricos y alertas en la app Blynk.

---

## Estructura del Proyecto

### Organización del Código
El proyecto mantiene una arquitectura modular con separación clara de responsabilidades:

- **Módulos de Sensores**: Implementaciones específicas para cada tipo de sensor
- **Controladores de Actuadores**: Gestión de bombas, ventiladores y válvulas
- **Gestión de Comunicaciones**: Manejo de WiFi, Blynk y protocolos serie
- **Lógica de Automatización**: Algoritmos de control y toma de decisiones
- **Utilidades del Sistema**: Funciones auxiliares y gestión de memoria


## Documentación Técnica

La documentación completa se encuentra en la carpeta [`docs/`](docs/):

- [Guía de Sensores](docs/sensores.md)
- [Guía de Actuadores](docs/actuadores.md)
- [Conexiones Hardware](docs/hardware_connections.md)
- [Configuración de Blynk](docs/setup_blynk.md)

Incluye diagramas, ejemplos y solución de problemas.

## Requisitos del Sistema

### Hardware Mínimo
- **ESP32 DevKit V1**: Microcontrolador principal con WiFi integrado
- **Fuente de Alimentación**: 12V DC con capacidad mínima de 5A
- **Sensores Especializados**: Conjunto completo según especificaciones técnicas
- **Módulo de Relés**: 8 canales con aislamiento óptico
- **Cableado Estructurado**: Cables blindados para conexiones de sensores

### Software y Plataformas
- **PlatformIO**: Entorno de desarrollo integrado
- **Arduino Framework**: Base de programación para ESP32
- **Blynk Legacy**: Plataforma de IoT para interfaz de usuario
- **Bibliotecas Especializadas**: Drivers para sensores y protocolos

## Estándares de Calidad

### Desarrollo de Software
El proyecto mantiene altos estándares de calidad mediante:

- **Análisis Estático**: Verificación continua con herramientas especializadas
- **Documentación Completa**: Código documentado según estándares industriales
- **Pruebas Unitarias**: Validación de funcionamiento de componentes individuales
- **Revisión de Código**: Proceso formal de revisión antes de integración

### Calidad del Hardware
- **Protección Eléctrica**: Fusibles y protecciones en todos los circuitos
- **Aislamiento Galvánico**: Separación entre circuitos de control y potencia
- **Certificación IP**: Protección contra humedad y polvo
- **Compatibilidad Electromagnética**: Cumplimiento de normas EMC


## Contribución y Desarrollo

### Proceso de Contribución
El proyecto acepta contribuciones siguiendo un proceso formal que incluye:

- **Estándares de Código**: Convenciones específicas para nomenclatura y estructura
- **Revisión Técnica**: Proceso de validación por pares
- **Documentación Obligatoria**: Actualización de documentación con cada cambio
- **Pruebas Requeridas**: Validación funcional antes de integración


## Licenciamiento
Este proyecto se distribuye bajo licencia MIT, permitiendo uso comercial y modificación con atribución apropiada. Consulta el archivo [LICENSE](LICENSE) para más detalles.

---

<p align="center">
  <b>¿Dudas o sugerencias?</b> Abre un issue o consulta la documentación técnica.
</p>

---

**Nota**: Para información detallada sobre instalación, configuración y uso del sistema, consulte la documentación técnica completa en el directorio `docs/`.