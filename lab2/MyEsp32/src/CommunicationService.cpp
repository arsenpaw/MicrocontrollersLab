#include "CommunicationService.h"
#include <thread>

CommunicationService::CommunicationService(HardwareSerial& serial, uint32_t baudRate)
    : serial(serial), baudRate(baudRate)
{
    serial.begin(115200, SERIAL_6E2,16,17);

}

void CommunicationService::send(ToogleCommand command)
{
    serial.write((uint8_t)command);
    Serial.print("Sent data: ");
    Serial.println((uint8_t)command);
}

void CommunicationService::onReceive(CommandDelegate commandDelegate)
{
    if (serial.available())
    {
        int receivedData = serial.read(); 
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
    serial.end();
}