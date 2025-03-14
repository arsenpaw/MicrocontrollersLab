#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include "AsyncStop.h"
#include "WebServerBase.h"
#include "ButtonWebServer.h"

ButtonWebServer::ButtonWebServer(const char* ssid, const char* password, CommunicationService communicationService)
    : WebServerBase(80), wifiSSID(ssid), wifiPassword(password), communicationService(communicationService)
{
}

void ButtonWebServer::init()
{
    initAP(wifiSSID, wifiPassword);
    setupRoutes();
    server.on("/press", HTTP_POST, [this]() { this->handleButtonStatus(); });
    server.on("/pressRemote", HTTP_POST, [this]() { this->handleRemoteButtonStatus(); });
    start();
}

void ButtonWebServer::handleReactRouting()
{
    server.on("/", HTTP_GET, [this]() {
        server.send(200, "text/html", "<form action=\"/press\" method=\"post\"><button>Press me</button></form>");
    });
}

void ButtonWebServer::handleButtonStatus()
{
    Serial.println("Button status requested");
    AsyncStop::getInstance().request();
    server.send(200, "text/plain", "Button pressed");
}

void ButtonWebServer::handleRemoteButtonStatus()
{
    communicationService.send(ToogleCommand::ON);
}


