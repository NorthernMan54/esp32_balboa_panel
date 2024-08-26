#include "spaWebServer.h"

#include <ESPAsyncWebServer.h>
#include <ArduinoLog.h>
#include <spaMessage.h>

// Local functions

void handleConfig(AsyncWebServerRequest *request);
void handleRoot(AsyncWebServerRequest *request);
void handleNotFound(AsyncWebServerRequest *request);

AsyncWebServer server(80);
bool serverSetup = false;

void spaWebServerSetup()
{
  // put your setup code here, to run once:
  Log.verbose(F("spaWebServerSetup()" CR));
}

void spaWebServerLoop()
{
  if (!serverSetup)
  {
    server.on("/", HTTP_GET, handleRoot);
    server.on("/config", HTTP_GET, handleConfig);
    server.onNotFound(handleNotFound);

    server.begin();
    serverSetup = true;
    Log.notice(F("Web server started at http://%p/" CR), WiFi.localIP());
  }
  // put your main code here, to run repeatedly:
  // Log.verbose(F("spaWebServerLoop()" CR));
}

void handleRoot(AsyncWebServerRequest *request)
{
  Log.verbose("Request %s received from %p" CR, request->url().c_str(), request->client()->remoteIP());
  String html = "<html><body><h1>Spa Status</h1><ul>";
  html += "<li>lastUpdate: " + String(spaStatusData.lastUpdate) + "</li>";
  html += "<li>magicNumber: " + String(spaStatusData.magicNumber) + "</li>";

  html += "<li>Current Temp: " + String(spaStatusData.currentTemp) + "°C</li>";
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
  Log.verbose(F("Response sent %s" CR), html.c_str());
}

void handleNotFound(AsyncWebServerRequest *request)
{
  Log.verbose(F("handleNotFound()" CR));
  request->send(404, "text/plain", "Not found");
}

void handleConfig(AsyncWebServerRequest *request)
{
  Log.verbose("Request %s received from %p" CR, request->url().c_str(), request->client()->remoteIP());

  String html = "<html><body><h1>Spa Configuration</h1><ul>";
  if (spaConfigurationData.lastUpdate == 0)
  {
    html += "<li>Spa Configuration not available</li>";
  }
  else
  {
    html += "<li>lastUpdate: " + String(spaConfigurationData.lastUpdate) + "</li>";
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
  Log.verbose(F("Response sent %s" CR), html.c_str());
}