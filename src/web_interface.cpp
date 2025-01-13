#include "web_interface.h"
#include <Arduino.h>

AsyncWebServer server(80);

extern bool dhtConnected;
extern bool tslConnected;
extern bool pirState; // Ajout de la variable pour le capteur PIR

const size_t TOTAL_HEAP = 81920; // Taille totale approximative de la RAM (80 kB)

String formatUptime(unsigned long millis)
{
    unsigned long seconds = millis / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    unsigned long days = hours / 24;

    seconds %= 60;
    minutes %= 60;
    hours %= 24;

    return String(days) + "j " + String(hours) + "h " + String(minutes) + "m " + String(seconds) + "s";
}

String formatMemory(size_t bytes)
{
    float megabytes = bytes / 1024.0 / 1024.0;
    return String(megabytes, 2) + " Mo";
}

void setupWebServer()
{
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        String html = "<!DOCTYPE html>";
        html += "<html>";
        html += "<head>";
        html += "<meta charset=\"UTF-8\">";
        html += "<title>ESP8266 Sensors</title>";
        html += "<style>";
        html += "body { font-family: Arial, sans-serif; margin: 0; padding: 20px; background-color: #f4f4f9; color: #333; }";
        html += "h1 { color: #005792; }";
        html += "p { line-height: 1.6; }";
        html += ".status { margin: 10px 0; padding: 10px; border-radius: 5px; }";
        html += ".connected { background-color: #d4edda; color: #155724; }";
        html += ".disconnected { background-color: #f8d7da; color: #721c24; }";
        html += "</style>";
        html += "</head>";
        html += "<body>";
        html += "<h1>ESP8266 - Données des capteurs</h1>";
        
        html += "<h2>Capteurs</h2>";
        html += "<p>Température : " + String(temperature, 1) + " °C</p>";
        html += "<p>Humidité : " + String(humidity, 1) + " %</p>";
        html += "<p>Luminosité : " + String(light, 1) + " lux</p>";
        
        html += "<h2>Statut des capteurs</h2>";
        html += "<p class='status " + String(dhtConnected ? "connected" : "disconnected") + "'>DHT : " + String(dhtConnected ? "Connecté" : "Déconnecté") + "</p>";
        html += "<p class='status " + String(tslConnected ? "connected" : "disconnected") + "'>TSL2561 : " + String(tslConnected ? "Connecté" : "Déconnecté") + "</p>";
        html += "<p class='status " + String(pirState ? "connected" : "disconnected") + "'>PIR : " + String(pirState ? "Mouvement détecté" : "Aucun mouvement") + "</p>"; // Nouveau statut PIR ajouté
        
        html += "<h2>Performances de la carte</h2>";
        html += "<p>Uptime : " + formatUptime(millis()) + "</p>";
        size_t freeHeap = ESP.getFreeHeap();
        size_t usedHeap = TOTAL_HEAP - freeHeap;
        html += "<p>Utilisation de la mémoire : " + formatMemory(usedHeap) + " utilisée / " + formatMemory(TOTAL_HEAP) + " totale</p>";
        html += "<p>Puissance du signal Wi-Fi : " + String(WiFi.RSSI()) + " dBm</p>";
        
        html += "</body>";
        html += "</html>";

        request->send(200, "text/html", html); });

    server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/plain", "Données mises à jour."); });

    server.begin();
}
