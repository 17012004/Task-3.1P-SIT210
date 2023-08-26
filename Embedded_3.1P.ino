#include <DHT.h>
#include <WiFiNINA.h>

char ssid[] = "Veekay";
char password[] = "12345678";

// ThingSpeak settings
char server[] = "api.thingspeak.com";
String apiKey = "9KFYF76U42FPGYHK";

// Pin configuration for DHT11 sensor
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Initialize the WiFi client
WiFiClient client;

void setup() {
 
  Serial.begin(9600);

  // Connect to WiFi
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting...");
    WiFi.begin(ssid, password);
    delay(10000); 
  }

  Serial.println("Connected to WiFi");
  dht.begin();
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  //data to send to ThingSpeak
  String data = "api_key=" + apiKey;
  data += "&field1=" + String(temperature);
  data += "&field2=" + String(humidity);

  //HTTP POST request to ThingSpeak
  if (client.connect(server, 80)) {
    client.print("POST /update HTTP/1.1\r\n");
    client.print("Host: " + String(server) + "\r\n");
    client.print("Content-Type: application/x-www-form-urlencoded\r\n");
    client.print("Content-Length: " + String(data.length()) + "\r\n");
    client.print("\r\n");
    client.print(data);
    
    Serial.println("Data sent to ThingSpeak!");
  } else {
    Serial.println("Connection to ThingSpeak failed!");
  }

  while (client.connected()) {
    if (client.available()) {
      char vee = client.read();
      Serial.print(vee);
    }
  }
  client.stop();

  // Wait before sending the next set of data
  delay(16000); 
}
