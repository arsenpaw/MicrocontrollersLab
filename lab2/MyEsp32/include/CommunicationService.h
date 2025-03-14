#pragma once
#include "ToogleCommand.h"
#include <Arduino.h>
class CommunicationService
{
public:
    using CommandDelegate = std::function<void(ToogleCommand)>;
    CommunicationService(HardwareSerial& serial, uint32_t baudRate);
    virtual ~CommunicationService();
    void send(ToogleCommand command);
    void onReceive(CommandDelegate commandDelegate);
private:
    HardwareSerial& communicationSerial; 
    uint32_t baudRate = 115200;
};