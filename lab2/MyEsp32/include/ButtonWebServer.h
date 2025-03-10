#ifndef BUTTONWEBSERVER_H
#define BUTTONWEBSERVER_H

#include "WebServerBase.h"

class ButtonWebServer : public WebServerBase {
public:
    ButtonWebServer(const char* ssid, const char* password);
    virtual void init();
    virtual void handleRoot() override;
    virtual void handleButtonStatus();
private:
    const char* wifiSSID;
    const char* wifiPassword;
};

#endif
