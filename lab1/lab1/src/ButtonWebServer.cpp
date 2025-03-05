#include "ButtonWebServer.h"
#include <WiFi.h>
#include <AsyncStop.h>
#include <WebServer.h>
ButtonWebServer::ButtonWebServer(const char* wifiSSID, const char* wifiPassword): 
    server(80), wifiSSID(wifiSSID), wifiPassword(wifiPassword), apIP(192, 168, 4, 1) {
}
void ButtonWebServer::init() {
    IPAddress subnet(255, 255, 255, 0);

    // Stop any existing WiFi connections
    WiFi.disconnect(true);
    
    // Start in AP mode
    WiFi.mode(WIFI_AP);
    
    // Start the AP
    if (!WiFi.softAPConfig(apIP, apIP, subnet)) {
        Serial.println("AP Config failed");
    }
    
    if (WiFi.softAP(wifiSSID, wifiPassword)) {
        Serial.println("Access Point Started");
        Serial.print("AP SSID: ");
        Serial.println(wifiSSID);
        Serial.print("AP IP address: ");
        Serial.println(WiFi.softAPIP());
    } else {
        Serial.println("AP Failed to start");
    }
    server.on("/", HTTP_GET, [this]() { this->handleRoot(); });
    server.on("/press", HTTP_POST, [this]() { this->handleButtonStatus(); });
    server.onNotFound([this]() { this->handleNotFound(); });
    
    // Start server
    server.begin();
    Serial.println("HTTP server started");
}
void ButtonWebServer::handleRoot() {
    String html = "<!DOCTYPE html>"
                  "<html>"
                  "<head><title>Button Status</title></head>"
                  "<body>"
                  "<h1>Button Web Server</h1>"
                  "<button onclick='sendRequest()'>Press Button</button>"
                  "<script>"
                  "function sendRequest() {"
                  "  fetch('/press', { method: 'POST' })"
                  "    .then(response => response.text())"
                  "    .then(data => console.log(data));"
                  "}"
                  "</script>"
                  "</body>"
                  "</html>";

    server.send(200, "text/html", html);
}


void ButtonWebServer::handleButtonStatus() {

    Serial.println("Button status requested");
    AsyncStop::getInstance().request();
    server.send(200, "text/plain");
}

void ButtonWebServer::handleNotFound() {
    server.send(404, "text/plain", "404: Not Found");
}

void ButtonWebServer::handleClient() {
    server.handleClient();
}