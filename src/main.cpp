#include "DHT.h"
#define DHTPIN 4    // Digital pin  #2 on ESP8266
#define DHTTYPE DHT22 // Sensor type 
#include <ESP8266WiFi.h>       // WiFi library
 
DHT dht(DHTPIN, DHTTYPE);
float humidity, temperature_C, temperature_F;

// Define settings
const char ssid[]     = ""; // WiFi SSID
const char pass[]     = ""; // WiFi password
const char domain[]   = "";  // domain.tmep.cz
const char guid[]     = "mereni"; // tmep variable
const byte sleep      = 1; // How often send data to the server. In minutes

void sendData(uint16_t teplota, uint16_t vlhkost) {
  WiFiClient client;

  char host[50];            // Joining two chars is little bit difficult. Make new array, 50 bytes long
  strcpy(host, domain);     // Copy /domain/ in to the /host/
  strcat(host, ".tmep.cz"); // Add ".tmep.cz" at the end of the /host/. /host/ is now "/domain/.tmep.cz"

  Serial.print(F("Connecting to ")); Serial.println(host);
  if (!client.connect(host, 80)) {
    Serial.println(F("Connection failed"));
    delay(1000);
    return;
  }
  Serial.println(F("Client connected"));

  // Make an url. We need: /?guid=t
  String url = "/?";
        url += guid;
        url += "=";
        url += teplota;
        url += "&humV=";
        url += vlhkost;
     

  Serial.print(F("Requesting URL: ")); Serial.println(url);

  // Make a HTTP GETrequest.
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
              "Host: " + host + "\r\n" + 
              "Connection: close\r\n\r\n");

  // Workaroud for timeout
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(F(">>> Client Timeout !"));
      client.stop();
      delay(1000);
      return;
    }
  }
}

void setup() {
  Serial.begin(9600);
  dht.begin();
   
  // Connect to the WiFi
  Serial.print(F("Connecting to ")); Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));
  Serial.print(F("IP address: ")); Serial.println(WiFi.localIP());
  Serial.println();

  delay(5000);
}

void loop() {
  delay(2000);

  temperature_C = dht.readTemperature();
    
  temperature_F = dht.readTemperature(true);

  humidity = dht.readHumidity();      
  
  // Check for error reading
  if (isnan(humidity) || isnan(temperature_C) || isnan(temperature_F)) {
    Serial.println(" DHT reading failed ");
    return;
  }


  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");
  
  Serial.print("Temperature:");
  Serial.print(temperature_C);
  Serial.print(("°C ------- "));
  
  Serial.print(temperature_F);
  Serial.println("°F");

  sendData(temperature_C, humidity);

  delay(sleep*60000);
}