Cada ESP32 obtiene su orientación y posición estimada, comparte sus coordenadas mediante WiFi y calcula:
* Distancia entre ambos dispositivos (fórmula Haversine)
* Dirección / Bearing hacia el otro dispositivo
* Comunicación directa ESP32 ↔ ESP32 bajo un mismo WiFi o usando WiFi Direct
Este proyecto es la base para una futura pulsera inteligente capaz de indicar dónde está la otra persona.

La fórmula de Haversine permite calcular la distancia entre dos puntos GPS (latitud/longitud) sobre la esfera terrestre.
Flujo de funcionamiento
El ESP32 se conecta por I2C al MPU9250.
Se obtiene:
Aceleración
Rotación
Campo magnético
Orientación (yaw/pitch/roll)
Posición estimada (o simulada)
El ESP32 en modo WiFi envia sus coordenadas al otro dispositivo.
Al recibir las coordenadas remotas:
Calcula la distancia mediante Haversine.
Calcula la dirección mediante atan2.

IMPORTANTE ---

Se debe conocer la MAC del otro ESP32. Para obtenerla, sube este código:
#include <WiFi.h>
void setup() { Serial.begin(115200); Serial.println(WiFi.macAddress()); }
void loop() {}
Si la MAC impresa es:
A4:CF:12:8B:3E:F0
Debe escribirse en el código así:
uint8_t peerMAC[] = {0xA4, 0xCF, 0x12, 0x8B, 0x3E, 0xF0};
Cada ESP32 debe poner en su código la MAC del otro dispositivo, no la propia.
