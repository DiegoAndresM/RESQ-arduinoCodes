#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"  // Librería para el cálculo de las pulsaciones
#include <WiFi.h>
#include <HTTPClient.h>

MAX30105 particleSensor;
uint32_t lastBeatTime;
float beatsPerMinute;
int beatAvg;

void setup() {
  Serial.begin(115200);

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("No se pudo encontrar un MAX30105. Verifique las conexiones.");
    while (1);
  }

  particleSensor.setup(0x02); // Configuración del LED a pulso de 411uA y frecuencia de muestreo de 100 Hz
  particleSensor.setPulseAmplitudeRed(0x0A); // Ajusta la amplitud del LED rojo para obtener buenos resultados
  particleSensor.setPulseAmplitudeIR(0x0A);  // Ajusta la amplitud del LED IR para obtener buenos resultados
  lastBeatTime = millis();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  uint32_t irValue = particleSensor.getIR();  // Lee el valor IR del sensor
  if (checkForBeat(irValue)) {  // Verifica si se detecta un latido
    // Si se detecta un latido, calcula las pulsaciones por minuto
    beatsPerMinute = 60 / ((millis() - lastBeatTime) / 1000.0);
    lastBeatTime = millis();

    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      // Filtra las variaciones abruptas en las pulsaciones
      beatAvg = (int)beatsPerMinute;
    }
  }

  Serial.print("Pulsaciones por minuto: ");
  Serial.println(beatAvg);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    String url = "https://api-nueva.vercel.app/api/BPM/" + cname + "/BPM/" + beatAvg;
    Serial.println("URL: " + url);
    
    http.begin(url);
    int httpCode = http.POST("");
    
    if (httpCode > 0) {
      Serial.printf("HTTP Response Code: %d\n", httpCode);
      String payload = http.getString();
      Serial.println("Response: " + payload);
    } else {
      Serial.println("Error on HTTP request");
    }
    
    http.end();
}
