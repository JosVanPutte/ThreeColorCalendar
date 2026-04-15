#include <WiFi.h>
#include <time.h>

const char* TZ_INFO = "CET-1CEST,M3.5.0,M10.5.0/3"; // De 'magic string' voor NL/BE

bool setupTime() {
  // Configureer tijd met NTP en de tijdzone string
  configTzTime(TZ_INFO, "pool.ntp.org", "time.nist.gov");
  
  struct tm timeinfo;
  return getLocalTime(&timeinfo);
}

uint64_t getSecondsToSleep(int targetHour) {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)) return 3600; // Bij fout: slaap een uur

  int currentHour = timeinfo.tm_hour;
  int currentMin  = timeinfo.tm_min;
  int currentSec  = timeinfo.tm_sec;

  // Bereken totaal seconden sinds middernacht
  long currentSeconds = (currentHour * 3600) + (currentMin * 60) + currentSec;
  long targetSeconds = targetHour * 3600;

  long sleepSeconds= (24 * 3600) - currentSeconds + targetSeconds;
  
  return (uint64_t)sleepSeconds;
}

struct tm getLocalTime(long timestamp) {
  // 1. Maak een time_t variabele van de long
    time_t rawTime = (time_t)timestamp;

    // 2. Zet de timestamp om naar een 'struct tm' (lokale tijdstructuur)
    // De functie localtime houdt automatisch rekening met je configTzTime instellingen!
    struct tm *timeinfo = localtime(&rawTime);
    return *timeinfo;
}