#include <HTTPClient.h>
#include <ArduinoJson.h> 

#include "google.h"
#include "keys.h"

const int maxEntryCount = 5;
struct calendarEntries calEnt[maxEntryCount];

const char *calendarRequest = CALENDAR_REQUEST;

HTTPClient http;

int calEntryCount;
struct calendarEntries *getCalendar(String prefix)
{
  calEntryCount = 0;
  // Getting calendar from your published google script
  Serial.println("Getting calendar");
  Serial.println(calendarRequest);

  http.end();
  http.setTimeout(20000);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  if (!http.begin(calendarRequest)) {
    Serial.println("Cannot connect to google script");
    return calEnt;
  } 

  Serial.println("Connected to google script");
  int returnCode = http.GET();
  Serial.print("Returncode: "); Serial.println(returnCode);
  String response = http.getString();
  Serial.print("Response: "); Serial.println(response);

  int indexFrom = 0;
  int indexTo = 0;
  int cutTo = 0;

  String strBuffer = "";

  int count = 0;
  int line = 0;

  Serial.println("IndexFrom");  
  indexFrom = response.lastIndexOf("\n") + 1;

  // Fill calendarEntries with entries from the get-request
  while (indexTo>=0 && line<maxEntryCount) {
    count++;
    indexTo = response.indexOf(";",indexFrom);
    cutTo = indexTo;

    if(indexTo != -1) { 
      strBuffer = response.substring(indexFrom, cutTo);
      
      indexFrom = indexTo + 1;
      
      Serial.println(strBuffer);

      if(count == 1) {
        // Set entry time
        calEnt[line].calTime = strBuffer.substring(0,21);
        if (prefix.isEmpty()) {
          prefix = strBuffer.substring(10);
        }

      } else if(count == 2) {
        // Set entry title
        calEnt[line].calTitle = strBuffer;

      } else {
          count = 0;
          Serial.printf("%s %s %s\n", calEnt[line].calTime.c_str(), calEnt[line].calTitle.c_str(),
            calEnt[line].calTime.startsWith(prefix) ? "TODAY": "not Today");
          if (calEnt[line].calTime.startsWith(prefix)) {
            ++line;
          }
      }
    }
  }
  calEntryCount = line;
  return calEnt;
}