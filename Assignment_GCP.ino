#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

const char* WIFI_SSID = "This is not a WiFi";
const char* WIFI_PASSWORD = "AWiFi@:)";
const char *MQTT_SERVER = "35.202.146.109"; // your VM instance public IP address
const int MQTT_PORT = 1883;
const char *MQTT_TOPIC = "iot";

#define DHT_PIN A2
#define MOTION_PIN A4
const int DHT_TYPE = DHT11;
const int RELAY_PIN = 38;
const int LED_PIN = 21;

DHT dht(DHT_PIN, DHT_TYPE);
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi()
{
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void setup()
{
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(MOTION_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.println("Attempting MQTT connection...");
  if (client.connect("ESP32Client"))
  {
    Serial.println("Connected to MQTT server");
  }
  else
  {
    Serial.print("Failed, rc=");
    Serial.print(client.state());
    Serial.println(" Retrying in 5 seconds...");
    delay(5000);
  }
  }
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }

  client.loop();
  delay(5000); // adjust the delay according to your requirements
  float temperature = dht.readTemperature();

  int motorStatus = digitalRead(RELAY_PIN);
  int motionStatus = digitalRead(MOTION_PIN);
  int ledStatus = digitalRead(LED_PIN);

  // Check temperature and control relay
  if (temperature > 30.0) {
    digitalWrite(RELAY_PIN, HIGH);
  } else {
    digitalWrite(RELAY_PIN, LOW);
  }

  // Check motion and control LED
  if (motionStatus == HIGH) {
    Serial.println("Motion Status");
    Serial.println(motionStatus);
    digitalWrite(LED_PIN, HIGH);
  } else {
    Serial.println("Motion Status");
    Serial.println(motionStatus);
    digitalWrite(LED_PIN, LOW);
  }

  char payload[40];  // Increased buffer size to accommodate both temperature and motion status
  sprintf(payload, "%.2f,%d,%d,%d", temperature, motorStatus, motionStatus, ledStatus);
  client.publish(MQTT_TOPIC, payload);
  delay(1000);  // Adjust the delay as needed
}
