#include <SoftwareSerial.h>
#include <WiFi.h>
#include <HTTPClient.h>

SoftwareSerial gps(16, 17); // Usar pines RX2 (16) y TX2 (17) en el ESP32

char dato = ' ';
String gprmcSentence; // Almacena la última sentencia GPRMC recibida
const char* ssid = "Galaxy S10";            // Nombre de tu red WiFi
const char* password = "contraseña";  // Contraseña de tu red WiFi

void setup() {
  Serial.begin(115200);
  gps.begin(9600, SWSERIAL_8N1, 16, 17); // Inicializar SoftwareSerial con pines RX2 y TX2

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  if (gps.available()) {
    dato = gps.read();
    gprmcSentence += dato; // Construir la sentencia GPRMC

    if (dato == '\n') { // Si es un carácter de nueva línea, procesar la sentencia
      if (gprmcSentence.startsWith("$GPRMC")) {
        processGPRMC(gprmcSentence); // Procesar la sentencia GPRMC
      }
      gprmcSentence = ""; // Reiniciar la sentencia para la próxima lectura
    }

    Serial.print(dato);
  }
  delay(5000)
}

void processGPRMC(String sentence) {
  // Buscar las posiciones de las comas en la sentencia GPRMC
  int comma1 = sentence.indexOf(',');
  int comma2 = sentence.indexOf(',', comma1 + 1);
  int comma3 = sentence.indexOf(',', comma2 + 1);
  int comma4 = sentence.indexOf(',', comma3 + 1);
  int comma5 = sentence.indexOf(',', comma4 + 1);
  int comma6 = sentence.indexOf(',', comma5 + 1);
  int comma7 = sentence.indexOf(',', comma6 + 1);

  if (comma1 != -1 && comma2 != -1 && comma3 != -1 && comma4 != -1 && comma5 != -1 && comma6 != -1 && comma7 != -1) {
    String latitude = sentence.substring(comma3 + 1, comma4);
    String longitude = sentence.substring(comma5 + 1, comma6);

    // Imprimir latitud y longitud en un formato resaltado
    Serial.println("\n--------------------------------------------------");
    Serial.print("Latitud: ");
    Serial.println(latitude);
    Serial.print("Longitud: ");
    Serial.println(longitude);
    Serial.println("--------------------------------------------------");
  }

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    String url = "https://api-nueva.vercel.app/api/cord/" + cname + "/coordinates/" + latitude + "/" + longitude;
    Serial.println("URL: " + url);
    
    http.begin(url);
    int httpCode = http.POST(""); // Enviar solicitud POST vacía
    
    if (httpCode > 0) {
      Serial.printf("HTTP Response Code: %d\n", httpCode);
      String payload = http.getString();
      Serial.println("Response: " + payload);
    } else {
      Serial.println("Error on HTTP request");
    }
    
    http.end();
  }
}