/*
  Combination of my own written code and:
  - https://github.com/bportaluri/WiFiEsp/blob/master/examples/WebClient/WebClient.ino
  - https://github.com/knolleary/pubsubclient/tree/master/examples/mqtt_esp8266
  When SW 420 sensor is sensing a vibration, data is sending to broker (CloudMQTT) with MQTT.
*/

#include "WiFiEsp.h"
#include "PubSubClient.h"

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7); // RX, TX
#endif

const char* ssid = "YourSSID";
const char* password = "YourPassword";
const char* mqtt_server = "YourMqttServer";
const int mqtt_port = 12622;
const char *mqtt_user = "YourUsername";
const char *mqtt_pass = "YourPassword";
// Client connections cant have the same connection name
const char *mqtt_client_name = "senseVibrationClient";

WiFiEspClient espClient;
PubSubClient client(mqtt_server, mqtt_port, espClient);
char msg[50];
int value = 0;

// Digital pin SW-420 sensor
int EP = 9;

void setup() {
  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  Serial1.begin(9600);
  // initialize ESP module
  WiFi.init(&Serial1);
  setup_wifi();
}

void setup_wifi() {

  delay(10);
  // Connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connection failed.");
  } else {
    Serial.print("Connection successfull!");
    Serial.println();
  }
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connection failed. Make connection again.");
    Serial.println();
    WiFi.begin(ssid, password);
  }

  long measurement = TP_init();

  if (measurement > 1000) {
    if (!client.connected()) {
      reconnect();
    } else {
      ++value;
      snprintf (msg, 75, "1 #%ld", value);
      Serial.print("Publish message: ");
      Serial.println(msg);
      // Set Topic to vibration with payload 1
      client.publish("vibration", msg);
    }
  } else {
    Serial.println("No connection, because SW-420 sensor is not sensing a vibration.");
  }
  client.loop();
}

long TP_init() {
  delay(10);
  // Wait for the pin to get HIGH and returns measurement
  long measurement = pulseIn (EP, HIGH);
  return measurement;
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("senseVibrationClient", mqtt_user, mqtt_pass)) {
      Serial.println("Connected to CloudMQTT server");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" Try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
