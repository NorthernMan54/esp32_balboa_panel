#include "spaCommunication.h"
#include <Arduino.h>
#include <ArduinoLog.h>
#include <WiFi.h>
#include "../../src/utilities.h"
#include "../../src/main.h"

#define BALBOA_PORT 4257
#define SPA_WRITE_QUEUE 10
#define SPA_READ_QUEUE 10

QueueHandle_t spaWriteQueue;
QueueHandle_t spaReadQueue;

void spaCommunicationSetup()
{
  Log.verbose(F("spaCommunicationSetup()" CR));
  spaWriteQueue = xQueueCreate(SPA_WRITE_QUEUE, sizeof(spaWriteQueueMessage_t));
  spaReadQueue = xQueueCreate(SPA_READ_QUEUE, sizeof(spaReadQueueMessage_t));
};

// SPA Communication State's

WiFiClient client;

bool spaCommunicationLoop(IPAddress spaIP)
{
  if (!client.connected())
  {
    Log.verbose(F("Connecting to Spa %p" CR), spaIP);
    if (client.connect(spaIP, BALBOA_PORT))
    {
      Log.verbose(F("Connected to Spa %p" CR), spaIP);
    }
  }
  else if (client.available())
  {
    spaReadQueueMessage messageToSend;
    messageToSend.length = client.read(messageToSend.message, BALBOA_MESSAGE_SIZE);
    if (xQueueSend(spaReadQueue, &messageToSend, 0) != pdTRUE)
    {
      Log.error(F("SPA Read Queue full, dropped %s" CR), msgToString(messageToSend.message, messageToSend.length).c_str());
    }
    else
    {
      // Log.verbose(F("Data added to Read Queue %s" CR), msgToString(messageToSend.message, messageToSend.length).c_str());
    }
  }
  else if (uxQueueMessagesWaiting(spaWriteQueue) > 0)
  {
    spaWriteQueueMessage messageToSend;
    if (xQueueReceive(spaWriteQueue, &messageToSend, 0) == pdTRUE)
    {
      Log.verbose(F("Sending data to Spa %s" CR), msgToString(messageToSend.message, messageToSend.length).c_str());
      client.write(messageToSend.message, messageToSend.length);
    }
  }
  return client.connected();
};
