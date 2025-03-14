#pragma once
#include "ToogleCommand.h"
#include <Arduino.h>
#include <SoftwareSerial.h>
class CommunicationService
{
public:
    using CommandDelegate = std::function<void(ToogleCommand)>;
    CommunicationService(SoftwareSerial& serial, uint32_t baudRate);
    virtual ~CommunicationService();
    void send(ToogleCommand command);
    void onReceive(CommandDelegate commandDelegate);
private:
    SoftwareSerial& serial; 
    uint32_t baudRate = 115200;
};