#ifndef WEB_INTERFACE_H
#define WEB_INTERFACE_H

#include <ESPAsyncWebServer.h>

// Déclaration des variables globales
extern float temperature;
extern float humidity;
extern float light;

// Déclaration des fonctions
void setupWebServer();
void updateSensorData(float temperature, float humidity, float light);

#endif
