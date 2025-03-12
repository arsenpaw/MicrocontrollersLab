#include <WiFi.h>
#include <WebServer.h>
#include "AsyncStop.h"
#include "WebServerBase.h"
#include "ButtonWebServer.h"

ButtonWebServer::ButtonWebServer(const char* ssid, const char* password)
    : WebServerBase(80), wifiSSID(ssid), wifiPassword(password)
{
}
void ButtonWebServer::init()
{
    initAP(wifiSSID, wifiPassword);
    setupRoutes();
    server.on("/press", HTTP_POST, [this]() { this->handleButtonStatus(); });
    start();
}

void ButtonWebServer::handleRoot()
{
    String html = "<!DOCTYPE html>"
                  "<html>"
                  "<head><title>Button Status</title></head>"
                  "<body>"
                  "<h1>Button Web Server</h1>"
                  "<button onclick='sendRequest()'>Press Button</button>"
                  "<script>"
                  "function sendRequest(){"
                  "fetch('/press', { method: 'POST' })"
                  ".then(response=>response.text())"
                  ".then(data=>console.log(data));"
                  "}"
                  "</script>"
                  "</body>"
                  "</html>";
    server.send(200, "text/html", html);
}

void ButtonWebServer::handleButtonStatus()
{
    Serial.println("Button status requested");
    AsyncStop::getInstance().request();
    server.send(200, "text/plain", "Button pressed");
}
