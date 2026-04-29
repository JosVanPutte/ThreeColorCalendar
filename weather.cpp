#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include "keys.h"

const char *apiKey = API_KEY;
String city = "Vlaardingen";
String countryCode = "NL";
#define INFO Serial.printf
struct tm getLocalTime(long timestamp);

char *getTimeStr(long t) {
  struct tm ts = getLocalTime(t);
  char *buffer = (char *)malloc(6);
  sprintf(buffer, "%02d:%02d", ts.tm_hour, ts.tm_min);
  return buffer;
}

char iconFound[5];

const char *getIconFound() {
  return iconFound;
}

const char *getWeather() {

    HTTPClient http;
    char *weather = (char *) malloc(30);    
    // API URL samenstellen
    String url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + apiKey + "&units=metric";
    INFO("begin %s\n", url.c_str());
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      // 1. Filter instellen voor de /weather endpoint
      StaticJsonDocument<300> filter;
      filter["main"]["temp_max"] = true;
      filter["main"]["temp_min"] = true;
      filter["main"]["humidity"] = true;
      filter["weather"][0]["icon"] = true;
      filter["weather"][0]["description"] = true;
      filter["wind"]["speed"] = true;

      // 2. Data ophalen en parsen
      StaticJsonDocument<1024> doc;
      DeserializationError error = deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter));

      if (!error) {
          // Temperaturen (Huidige stations-uitersten)
          float maxTemp = doc["main"]["temp_max"];
          float minTemp = doc["main"]["temp_min"];
          int humidity  = doc["main"]["humidity"];

          // Weer details
          const char *icon = doc["weather"][0]["icon"];
          strcpy(iconFound, icon);
          const char* desc = doc["weather"][0]["description"];

          // Wind (m/s naar knopen: * 1.94384)
          float windMs = doc["wind"]["speed"];
          float windKnopen = windMs * 1.94384;

          // Output
      
          INFO("max %s\n", String(maxTemp).c_str());
          INFO("min %s\n", String(minTemp).c_str());
          INFO("Beschrijving: %s\n", desc);
          INFO("Icon %s\n", icon);
          INFO("Wind (Knopen): %s\n", String(windKnopen).c_str());
          INFO("Luchtvochtigheid: %s\n", String(humidity).c_str());
          snprintf(weather, 30, "%2.1f/%2.1fC %2.1fkts %2d%%",
            maxTemp, minTemp, windKnopen, humidity);
      }
      http.end();
    }
    return weather;
}
  
