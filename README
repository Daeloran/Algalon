# Light-and-Temperature-Sensor

## Description

Ce projet a pour but de permettre la récupération d'informations sur l'environnement (température, humidité et luminosité) à l'aide de capteurs connectés à un ESP8266. Les données collectées sont ensuite envoyées à un broker MQTT pour permettre à Home Assistant de les récupérer et de les utiliser.

## Matériel requis

-   ESP8266 (D1 Mini Pro)
-   Capteur de température et d'humidité DHT22
-   Capteur de luminosité TSL2561
-   Connexion Wi-Fi
-   Broker MQTT (par exemple Mosquitto)

## Bibliothèques nécessaires

-   `Arduino.h`
-   `Wire.h`
-   `Adafruit_Sensor.h`
-   `DHT.h`
-   `Adafruit_TSL2561_U.h`
-   `ESP8266WiFi.h`
-   `AsyncMqttClient.h`
-   `ESPAsyncWebServer.h`

## Configuration

1. Cloner ce dépôt sur votre machine locale.
2. Ouvrir le projet dans l'IDE Arduino.
3. Installer les bibliothèques nécessaires via le gestionnaire de bibliothèques de l'IDE Arduino.
4. Configurer les informations Wi-Fi et MQTT dans le fichier `main.cpp` :
    ```cpp
    const char *ssid = "Votre_SSID";
    const char *password = "Votre_Mot_de_passe";
    const char *mqtt_server = "Adresse_IP_du_broker_MQTT";
    const int mqtt_port = 1883;
    const char *mqtt_user = "Votre_Utilisateur_MQTT";
    const char *mqtt_password = "Votre_Mot_de_passe_MQTT";
    ```
5. Téléverser le code sur l'ESP8266.

## Utilisation

1. Connecter les capteurs DHT22 et TSL2561 à l'ESP8266 selon les broches définies dans le code.
2. Alimenter l'ESP8266.
3. Accéder à l'interface web pour visualiser les données des capteurs en temps réel.
4. Les données seront automatiquement publiées sur le broker MQTT configuré, prêtes à être récupérées par Home Assistant.

## Interface Web

L'interface web permet de visualiser les données des capteurs ainsi que l'état de la connexion Wi-Fi et l'utilisation de la mémoire de l'ESP8266. Elle est accessible à l'adresse IP de l'ESP8266 sur le port 80.

## Auteur

Daeloran

## Licence

Ce projet est sous licence MIT. Voir le fichier `LICENSE` pour plus de détails.
