#include "spaWebServer.h"

#include <ESPAsyncWebServer.h>
#include <ArduinoLog.h>
#include <base64.hpp>

// Internal libraries

#include <spaMessage.h>
#include "../../src/utilities.h"

// Local functions

void handleConfig(AsyncWebServerRequest *request);
void handleRoot(AsyncWebServerRequest *request);
void handleNotFound(AsyncWebServerRequest *request);
void handleBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
void handleData(AsyncWebServerRequest *request);

AsyncWebServer server(80);
bool serverSetup = false;

void spaWebServerSetup()
{
  // put your setup code here, to run once:
  Log.verbose(F("[Web]: spaWebServerSetup()" CR));
}

void spaWebServerLoop()
{
  if (!serverSetup)
  {
    server.on("/", HTTP_GET, handleRoot);
    server.on("/config", HTTP_GET, handleConfig);
    server.on("/data", HTTP_POST, handleData, {}, handleBody);
    server.on("/data", HTTP_OPTIONS, handleData, {}, handleBody);
    server.onNotFound(handleNotFound);

    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");

    server.begin();
    serverSetup = true;
    Log.notice(F("[Web]: Web server started at http://%p/" CR), WiFi.localIP());
  }
  // put your main code here, to run repeatedly:
  // Log.verbose(F("[Web]: spaWebServerLoop()" CR));
}

void handleRoot(AsyncWebServerRequest *request)
{
  Log.verbose("[Web]: Request %s received from %p" CR, request->url().c_str(), request->client()->remoteIP());
  String html = "<html><body><h1>Spa Status</h1><ul>";
  html += "<li>lastUpdate: " + formatNumberWithCommas(spaStatusData.lastUpdate) + "</li>";
  html += "<li>magicNumber: " + String(spaStatusData.magicNumber) + "</li>";
  html += "<br><li>Free Heap: " + formatNumberWithCommas(ESP.getFreeHeap()) + "</li>";
  html += "<li>Free Stack: " + formatNumberWithCommas(uxTaskGetStackHighWaterMark(NULL)) + "</li>";

  html += "<br><li>Current Temp: " + String(spaStatusData.currentTemp) + "°C</li>";
  html += "<li>Set Temp: " + String(spaStatusData.setTemp) + "°C</li>";
  html += "<li>Heating Mode: " + String(spaStatusData.heatingMode) + "</li>";
  html += "<li>Heating State: " + String(spaStatusData.heatingState) + "</li>";
  html += "<li>Needs Heat: " + String(spaStatusData.needsHeat) + "</li>";
  html += "<li>Temp Range: " + String(spaStatusData.tempRange) + "</li>";
  html += "<li>Temp Scale: " + String(spaStatusData.tempScale) + "</li>";
  html += "<li>Time: " + String(spaStatusData.time) + "</li>";
  html += "<li>Clock Mode: " + String(spaStatusData.clockMode) + "</li>";
  html += "<li>Filter Mode: " + String(spaStatusData.filterMode) + "</li>";
  html += "<li>Pump 1: " + String(spaStatusData.pump1) + "</li>";
  html += "<li>Pump 2: " + String(spaStatusData.pump2) + "</li>";
  html += "<li>Pump 3: " + String(spaStatusData.pump3) + "</li>";
  html += "<li>Pump 4: " + String(spaStatusData.pump4) + "</li>";
  html += "<li>Pump 5: " + String(spaStatusData.pump5) + "</li>";
  html += "<li>Pump 6: " + String(spaStatusData.pump6) + "</li>";
  html += "<li>Circulation Pump: " + String(spaStatusData.circ) + "</li>";
  html += "<li>Blower: " + String(spaStatusData.blower) + "</li>";
  html += "<li>Light 1: " + String(spaStatusData.light1) + "</li>";
  html += "<li>Light 2: " + String(spaStatusData.light2) + "</li>";
  html += "<li>Mister: " + String(spaStatusData.mister) + "</li>";
  html += "<li>Panel Locked: " + String(spaStatusData.panelLocked) + "</li>";
  // Add more fields as needed
  html += "</ul></body></html>";
  request->send(200, "text/html", html);
  Log.verbose(F("[Web]: Response sent %s" CR), html.c_str());
}

// <sci_request version="1.0"><file_system cache="false"><targets><device id="00 11 22 33 44 55 66 77" /></targets><commands><get_file path="SetupParameters.txt" /></commands></file_system></sci_request>
// <sci_request version="1.0"><file_system><targets><device id="00 11 22 33 44 55 66 77"/></targets><commands><get_file path="PanelUpdate.txt"/></commands></file_system></sci_request>
// <sci_request version="1.0"><data_service><targets><device id="00 11 22 33 44 55 66 77"/></targets><requests><device_request target_name="Request">Filters</device_request></requests></data_service></sci_request>
// <sci_request version="1.0"><file_system><targets><device id="00 11 22 33 44 55 66 77"/></targets><commands><get_file path="SystemInformation.txt"/></commands></file_system></sci_request>

String encodeResponse(uint8_t rawData[50], uint8_t length)
{
  unsigned char message[BALBOA_MESSAGE_SIZE];
  for (int i = 0; i < length; i++)
  {
    message[i] = (char)rawData[i];
  }
  message[length] = '\0';

  // Base64 encode the string
  unsigned char encoded[BALBOA_MESSAGE_SIZE * 2];
  int encodedLength = encode_base64(message, length, encoded);
  encoded[encodedLength] = '\0';
  char encodedString[BALBOA_MESSAGE_SIZE * 2];
  strncpy((char *)encodedString, (char *)encoded, encodedLength);
  encodedString[encodedLength] = '\0';
  Log.verbose("Encoded: %s\n", encodedString);
  return String(encodedString);
}

// <sci_request version="1.0"><file_system><targets><device id="00 11 22 33 44 55 66 77"/></targets><commands><get_file path="PanelUpdate.txt"/></commands></file_system></sci_request>
// <sci_request version="1.0"><file_system><targets><device id="00 11 22 33 44 55 66 77"/></targets><commands><get_file path="SystemInformation.txt"/></commands></file_system></sci_request>
// <sci_request version="1.0"><file_system cache="false"><targets><device id="00 11 22 33 44 55 66 77" /></targets><commands><get_file path="SetupParameters.txt" /></commands></file_system></sci_request>

// <sci_request version="1.0"><data_service><targets><device id="00 11 22 33 44 55 66 77"/></targets><requests><device_request target_name="Request">Filters</device_request></requests></data_service></sci_request>

void handleBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{

  String response = "";
  String body(reinterpret_cast<char *>(data), len);
  if (body.indexOf("PanelUpdate.txt") > 0)
  {
    response = encodeResponse(spaStatusData.rawData, spaStatusData.rawDataLength);
  }
  else if (body.indexOf("DeviceConfiguration.txt") > 0)
  {
    response = encodeResponse(spaConfigurationData.rawData, spaConfigurationData.rawDataLength);
  }
  else if (body.indexOf("SetupParameters.txt") > 0)
  {
    response = encodeResponse(spaPreferencesData.rawData, spaPreferencesData.rawDataLength);
  }
  else if (body.indexOf("SystemInformation.txt") > 0)
  {
    response = encodeResponse(spaInformationData.rawData, spaInformationData.rawDataLength);
  }
  else if (body.indexOf("Filters") > 0)
  {
    response = encodeResponse(spaFilterSettingsData.rawData, spaFilterSettingsData.rawDataLength);
  }
  else
  {
    Log.verbose("[Web]: Unknown object requested %s\n", body.c_str());
    request->send(404, "text/plain", "Not found");
    return;
  }

  if (response.length() > 0)
  {
    request->send(200, "text/xml", "<response><data>" + response + "</data></response>");
  }
  else
  {
      
    Log.verbose("[Web]: Empty response %s\n", body.c_str());
    request->send(404, "text/plain", "Not found");
  }
}

void handleData(AsyncWebServerRequest *request)
{
  Log.verbose("[Web]: handleData Request %s %s received from %p" CR, request->methodToString(), request->url().c_str(), request->client()->remoteIP());
  /*
  int headers = request->headers();
  int i;
  for (i = 0; i < headers; i++)
  {
    AsyncWebHeader *h = request->getHeader(i);
    Log.verbose("HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
  }
  int args = request->args();
  for (int i = 0; i < args; i++)
  {
    Log.verbose("ARG[%s]: %s" CR, request->argName(i).c_str(), request->arg(i).c_str());
  }
  */
  request->send(200, "text/plain", "Data received");
}

void handleNotFound(AsyncWebServerRequest *request)
{
  Log.verbose(F("[Web]: handleNotFound() %s %s" CR), request->methodToString(), request->url().c_str());
  int headers = request->headers();
  int i;
  for (i = 0; i < headers; i++)
  {
    AsyncWebHeader *h = request->getHeader(i);
    Log.verbose("HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
  }

  int args = request->args();
  for (int i = 0; i < args; i++)
  {
    Log.verbose(F("ARG[%s]: %s" CR), request->argName(i).c_str(), request->arg(i).c_str());
  }

  request->send(404, "text/plain", "Not found");
}

void handleConfig(AsyncWebServerRequest *request)
{
  Log.verbose("[Web]: Request %s received from %p" CR, request->url().c_str(), request->client()->remoteIP());

  String html = "<html><body><h1>Spa Configuration</h1><ul>";
  if (spaConfigurationData.lastUpdate == 0)
  {
    html += "<li>Spa Configuration not available</li>";
  }
  else
  {
    html += "<li>lastUpdate: " + formatNumberWithCommas(spaConfigurationData.lastUpdate) + "</li>";
    html += "<li>magicNumber: " + String(spaConfigurationData.magicNumber) + "</li>";
    html += "<li>Pump 1: " + String(spaConfigurationData.pump1) + "</li>";
    html += "<li>Pump 2: " + String(spaConfigurationData.pump2) + "</li>";
    html += "<li>Pump 3: " + String(spaConfigurationData.pump3) + "</li>";
    html += "<li>Pump 4: " + String(spaConfigurationData.pump4) + "</li>";
    html += "<li>Pump 5: " + String(spaConfigurationData.pump5) + "</li>";
    html += "<li>Pump 6: " + String(spaConfigurationData.pump6) + "</li>";
    html += "<li>Light 1: " + String(spaConfigurationData.light1) + "</li>";
    html += "<li>Light 2: " + String(spaConfigurationData.light2) + "</li>";
    html += "<li>Blower: " + String(spaConfigurationData.blower) + "</li>";
    html += "<li>Circulation Pump: " + String(spaConfigurationData.circulationPump) + "</li>";
    html += "<li>Aux 1: " + String(spaConfigurationData.aux1) + "</li>";
    html += "<li>Aux 2: " + String(spaConfigurationData.aux2) + "</li>";
    html += "<li>Mister: " + String(spaConfigurationData.mister) + "</li>";
    html += "<li>temp_scale: " + String(spaConfigurationData.temp_scale) + "</li>";

    // Add more fields as needed
    html += "</ul></body></html>";
  }
  request->send(200, "text/html", html);
  Log.verbose(F("[Web]: Response sent %s" CR), html.c_str());
}