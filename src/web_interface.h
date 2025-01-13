#ifndef WEB_INTERFACE_H
#define WEB_INTERFACE_H

#include <ESPAsyncWebServer.h>

// Déclaration des variables globales
extern float temperature;
extern float humidity;
extern float light;
extern bool pirState; // Ajout de la variable pour l'état du capteur PIR

// Déclaration des fonctions
void setupWebServer();
void updateSensorData(float temperature, float humidity, float light);
void updatePirState(bool state); // Nouvelle fonction pour mettre à jour l'état PIR

#endif
