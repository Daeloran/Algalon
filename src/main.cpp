#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Adafruit_TSL2561_U.h>
#include <ESP8266WiFi.h>
#include <AsyncMqttClient.h>
#include "web_interface.h"

// Configuration Wi-Fi et MQTT
const char *ssid = "MurlocCompany";
const char *password = "MurlocCompany160107Lupink13Shynael04";
const char *mqtt_server = "192.168.1.29";
const int mqtt_port = 1883;
const char *mqtt_user = "TheSpeaker";
const char *mqtt_password = "h45T135t-R3t-3rUct1n6-1hrAm";

// Définition des variables globales
float temperature = NAN;
float humidity = NAN;
float light = NAN;

// Définir les broches et capteurs
#define DHTPIN D4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

// Gestion Wi-Fi et MQTT
WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
AsyncMqttClient mqttClient;

// Variables pour la gestion des états et intervalles
bool dhtConnected = false;
bool tslConnected = false;
unsigned long lastWifiCheck = 0;
unsigned long lastSensorCheck = 0;
unsigned long lastPublish = 0;
unsigned long lastMqttReconnectAttempt = 0;
const unsigned long wifiCheckInterval = 30000;
const unsigned long sensorCheckInterval = 30000;
const unsigned long publishInterval = 30000;
const unsigned long mqttReconnectInterval = 5000;

bool pendingPublish = false; // Indique si une publication est en attente

void connectToWifi()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Connexion au Wi-Fi...");
    WiFi.begin(ssid, password);
    unsigned long startAttemptTime = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 20000)
    {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("\nConnecté au Wi-Fi");
      Serial.print("Adresse IP: ");
      Serial.println(WiFi.localIP());
    }
    else
    {
      Serial.println("\nÉchec de la connexion Wi-Fi");
    }
  }
}

void connectToMqtt()
{
  if (pendingPublish && !mqttClient.connected() && millis() - lastMqttReconnectAttempt >= mqttReconnectInterval)
  {
    lastMqttReconnectAttempt = millis();
    Serial.println("Tentative de connexion au broker MQTT...");
    mqttClient.connect();
  }
}

void onWifiConnect(const WiFiEventStationModeGotIP &event)
{
  Serial.println("Connecté au Wi-Fi !");
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected &event)
{
  Serial.println("Wi-Fi déconnecté. Reconnexion...");
  connectToWifi();
}

void onMqttConnect(bool sessionPresent)
{
  Serial.println("Connecté au broker MQTT !");
  pendingPublish = false; // Une fois connecté, réinitialise l'état de publication
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
  Serial.print("Déconnecté du broker MQTT. Raison : ");
  switch (reason)
  {
  case AsyncMqttClientDisconnectReason::TCP_DISCONNECTED:
    Serial.println("Déconnexion TCP.");
    break;
  case AsyncMqttClientDisconnectReason::MQTT_UNACCEPTABLE_PROTOCOL_VERSION:
    Serial.println("Version du protocole MQTT inacceptable.");
    break;
  case AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED:
    Serial.println("Identifiant rejeté.");
    break;
  case AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE:
    Serial.println("Serveur MQTT indisponible.");
    break;
  case AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS:
    Serial.println("Nom d'utilisateur ou mot de passe incorrect.");
    break;
  case AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED:
    Serial.println("Non autorisé.");
    break;
  case AsyncMqttClientDisconnectReason::ESP8266_NOT_ENOUGH_SPACE:
    Serial.println("Espace insuffisant sur l'ESP8266.");
    break;
  case AsyncMqttClientDisconnectReason::TLS_BAD_FINGERPRINT:
    Serial.println("Empreinte digitale TLS incorrecte.");
    break;
  default:
    Serial.println("Raison inconnue.");
    break;
  }
}

void checkSensors()
{
  // Lecture DHT
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (!isnan(h) && !isnan(t))
  {
    dhtConnected = true;
    temperature = t;
    humidity = h;
    Serial.printf("Lecture DHT : Temp=%.1f°C, Hum=%.1f%%\n", temperature, humidity);
  }
  else
  {
    dhtConnected = false; // Si la lecture échoue, le DHT sera réinitialisé au prochain cycle
    dht.begin();          // Tenter une réinitialisation
    Serial.println("Échec de lecture du capteur DHT. Réinitialisation...");
  }

  // Lecture TSL2561
  if (!tslConnected)
  {
    if (tsl.begin())
    {
      tslConnected = true;
      tsl.enableAutoRange(true);
      tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);
      Serial.println("Capteur TSL2561 reconnecté !");
    }
    else
    {
      Serial.println("Échec de reconnexion au TSL2561.");
    }
  }

  if (tslConnected)
  {
    sensors_event_t event;
    tsl.getEvent(&event);
    if (event.light)
    {
      light = event.light;
      Serial.printf("Lecture TSL2561 : Lumière=%.2f lux\n", light);
    }
    else
    {
      tslConnected = false; // Si la lecture échoue, forcer une nouvelle tentative de connexion
      Serial.println("Échec de lecture du capteur TSL2561. Tentative de reconnexion...");
    }
  }
}

void publishSensorData()
{
  if (mqttClient.connected())
  {
    if (dhtConnected)
    {
      mqttClient.publish("sensor/temperature", 0, false, String(temperature).c_str());
      mqttClient.publish("sensor/humidity", 0, false, String(humidity).c_str());
      Serial.printf("Données DHT publiées : Temp=%.1f°C, Hum=%.1f%%\n", temperature, humidity);
    }
    if (tslConnected)
    {
      mqttClient.publish("sensor/light", 0, false, String(light).c_str());
      Serial.printf("Données TSL2561 publiées : Lumière=%.2f lux\n", light);
    }
  }
  else
  {
    pendingPublish = true; // Marque la publication comme en attente
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("\nDémarrage...");

  // Configuration Wi-Fi
  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
  connectToWifi();

  // Initialisation du capteur DHT
  dht.begin();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t))
  {
    Serial.println("Échec de connexion ou lecture du capteur DHT.");
  }
  else
  {
    Serial.println("Capteur DHT connecté !");
  }

  // Initialisation du capteur TSL2561
  if (!tsl.begin())
  {
    Serial.println("Échec de connexion au capteur TSL2561.");
  }
  else
  {
    tsl.enableAutoRange(true);
    tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);
    Serial.println("Capteur TSL2561 connecté !");
  }

  // Configuration MQTT
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCredentials(mqtt_user, mqtt_password);
  mqttClient.setClientId("D1MiniProClient");

  // Configuration et démarrage du serveur web
  setupWebServer();
}

void loop()
{
  unsigned long currentMillis = millis();

  // Vérification Wi-Fi
  if (currentMillis - lastWifiCheck >= wifiCheckInterval)
  {
    lastWifiCheck = currentMillis;
    connectToWifi();
  }

  // Tentative de connexion MQTT
  connectToMqtt();

  // Lecture des capteurs
  if (currentMillis - lastSensorCheck >= sensorCheckInterval)
  {
    lastSensorCheck = currentMillis;
    checkSensors();
  }

  // Publication des données si nécessaire
  if (currentMillis - lastPublish >= publishInterval)
  {
    lastPublish = currentMillis;
    publishSensorData();
  }
}
