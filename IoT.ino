#include <WiFi.h>
#include <esp_now.h>

// ---------------------------------------------------
// 1. Estructura que enviamos/recibimos
// ---------------------------------------------------
typedef struct {
  float lat;
  float lon;
  float heading;  // usaremos después
} DatosGPS;

DatosGPS mis_datos;
DatosGPS datos_remoto;

// ---------------------------------------------------
// 2. MAC del otro ESP32  (CAMBIA ESTA)
// ---------------------------------------------------
uint8_t mac_remota[] = {0x24, 0x6F, 0x28, 0x12, 0x34, 0x56};

// ---------------------------------------------------
// CALLBACK: Cuando SE ENVÍAN datos
// ---------------------------------------------------
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Enviado: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "OK" : "ERROR");
}

// ---------------------------------------------------
// CALLBACK: Cuando se RECIBEN datos
// ---------------------------------------------------
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&datos_remoto, incomingData, sizeof(datos_remoto));

  Serial.println("\n--- Datos recibidos ---");
  Serial.print("Lat: "); Serial.println(datos_remoto.lat, 6);
  Serial.print("Lon: "); Serial.println(datos_remoto.lon, 6);

  // Calcular distancia y dirección
  float distancia = haversine(mis_datos.lat, mis_datos.lon,
                              datos_remoto.lat, datos_remoto.lon);

  float bearing = calcularBearing(mis_datos.lat, mis_datos.lon,
                                  datos_remoto.lat, datos_remoto.lon);

  Serial.print("Distancia (m): "); Serial.println(distancia);
  Serial.print("Direccion (°): "); Serial.println(bearing);
}

// ---------------------------------------------------
// 3. Fórmula de Haversine (distancia)
// ---------------------------------------------------
float haversine(float lat1, float lon1, float lat2, float lon2) {
  float R = 6371000; // metros
  float dLat = radians(lat2 - lat1);
  float dLon = radians(lon2 - lon1);

  float a = sin(dLat/2) * sin(dLat/2) +
            cos(radians(lat1)) * cos(radians(lat2)) *
            sin(dLon/2) * sin(dLon/2);

  float c = 2 * atan2(sqrt(a), sqrt(1-a));
  return R * c;
}

// ---------------------------------------------------
// 4. Bearing (dirección hacia el otro punto)
// ---------------------------------------------------
float calcularBearing(float lat1, float lon1, float lat2, float lon2) {
  lat1 = radians(lat1);
  lat2 = radians(lat2);
  float dLon = radians(lon2 - lon1);

  float x = sin(dLon) * cos(lat2);
  float y = cos(lat1)*sin(lat2) -
            sin(lat1)*cos(lat2)*cos(dLon);

  float brng = atan2(x, y);
  brng = degrees(brng);
  if (brng < 0) brng += 360;
  return brng;
}

// ---------------------------------------------------
// SETUP
// ---------------------------------------------------
void setup() {
  Serial.begin(115200);

  // 1. Poner WiFi en modo STA
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // 2. Inicializar ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error inicializando ESP-NOW");
    return;
  }

  // Registrar callbacks
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  // Registrar peer (el otro ESP32)
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, mac_remota, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Error agregando peer");
    return;
  }

  // Simulación de coordenadas (CAMBIA ESTO luego con GPS)
  mis_datos.lat = 19.432608;  // CDMX
  mis_datos.lon = -99.133209;
  mis_datos.heading = 0; // luego vendrá del MPU
}

// ---------------------------------------------------
// LOOP
// ---------------------------------------------------
void loop() {
  // Enviar nuestros datos cada 1 segundo
  esp_now_send(mac_remota, (uint8_t*)&mis_datos, sizeof(mis_datos));
  delay(1000);
}
