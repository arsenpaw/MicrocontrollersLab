#include "CommunicationService.h"
#include <thread>

CommunicationService::CommunicationService(HardwareSerial& serial, uint32_t baudRate)
    : communicationSerial(serial), baudRate(baudRate)
{


}

void CommunicationService::init()
{
    communicationSerial.begin(baudRate, SERIAL_6E2,16,17);
}
void CommunicationService::send(ToogleCommand command)
{
    communicationSerial.write((uint8_t)command);
    Serial.print("Sent data: ");
    Serial.println((uint8_t)command);
}

void CommunicationService::onReceive(CommandDelegate commandDelegate)
{
    if (communicationSerial.available())
    {
        int receivedData = communicationSerial.read(); 
        Serial.print("Received data: ");
        Serial.println(receivedData);
        if (receivedData == (uint8_t)ToogleCommand::ON || 
            receivedData == (uint8_t)ToogleCommand::OFF) {
            commandDelegate((ToogleCommand)receivedData);
        }
    }
}

CommunicationService::~CommunicationService()
{
    communicationSerial.end();
}