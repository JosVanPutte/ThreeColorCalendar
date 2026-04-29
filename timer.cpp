#include <WiFi.h>
#include <time.h>

const char* TZ_INFO = "CET-1CEST,M3.5.0,M10.5.0/3"; // De 'magic string' voor NL/BE
char timeStringBuff[12];

struct tm timeinfo;

const char *setupTime() {
  // Configureer tijd met NTP en de tijdzone string
  configTzTime(TZ_INFO, "pool.ntp.org", "time.nist.gov");
  
  if (getLocalTime(&timeinfo)) {
    // Formaat: Dag-naam Maand-naam Dag-getal
    strftime(timeStringBuff, sizeof(timeStringBuff), "%a %b %d", &timeinfo);
  }
  return timeStringBuff;
}
int getNextWakeHour(int currentHour) {
  int targetHours[] = {6, 9, 12, 15, 18, 21};
  int numTargets = 6;

  for (int i = 0; i < numTargets; i++) {
    // Zoek het eerste uur in de lijst dat groter is dan nu
    if (targetHours[i] > currentHour) {
      return targetHours[i];
    }
  }

  // Als we hier komen, is het na 21:00. 
  // Het volgende wekmoment is 6:00 de volgende dag.
  return 6; 
}

long getSecondsToSleep() {
  if(!getLocalTime(&timeinfo)) return 3600;
  int currentHour = timeinfo.tm_hour;
  int currentMin  = timeinfo.tm_min;
  int currentSec  = timeinfo.tm_sec;
  int targetHour = getNextWakeHour(currentHour);
  
  // Bereken totaal seconden sinds middernacht
  long currentSeconds = (currentHour * 3600) + (currentMin * 60) + currentSec;
  long targetSeconds = targetHour * 3600;

  long sleepSeconds= targetSeconds - currentSeconds;
  if (sleepSeconds < 0) {
    sleepSeconds += 24 * 3600;
  }

  return sleepSeconds;
}

struct tm getLocalTime(long timestamp) {
  // 1. Maak een time_t variabele van de long
    time_t rawTime = (time_t)timestamp;

    // 2. Zet de timestamp om naar een 'struct tm' (lokale tijdstructuur)
    // De functie localtime houdt automatisch rekening met je configTzTime instellingen!
    struct tm *timeinfo = localtime(&rawTime);
    return *timeinfo;
}