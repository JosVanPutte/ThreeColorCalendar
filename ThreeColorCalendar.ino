#include <WiFi.h>
#include <ArduinoOTA.h>
#include "google.h"
#include "keys.h"

const char *scriptId = SCRIPTID;
const char *ssid = SSID;
const char *password = PASSWORD;

esp_sleep_wakeup_cause_t wakeup_reason;
const char *getWeather();
void goToDeepSleep();
const char *setupTime();
void setupDisplay(struct calendarEntries *entries, const char *w);
uint64_t getSecondsToSleep(int targetHour);

void startOTAMode();

void setup() {
  // Start serieel voor debugging
  Serial.begin(115200);
  delay(200); 
  Serial.println("Systeem start op...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(500);
  }
  // Check waarom we wakker zijn geworden
  wakeup_reason = esp_sleep_get_wakeup_cause();

  if (wakeup_reason != ESP_SLEEP_WAKEUP_TIMER) {
    Serial.println("Start OTA Modus...");
    // Blijf hier bijv. 2 minuten wachten op OTA
    startOTAMode(); 
  }
  String today = setupTime();
  if (today.isEmpty()) {
    Serial.println("Mislukt om tijd op te halen");
  }
  struct calendarEntries *entries = getCalendar(scriptId, today);
  const char * wthr = getWeather();
  setupDisplay(entries, wthr);
  if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER) {
    goToDeepSleep();
  }
}

/**
Jumper,Toestand,Resultaat
A (AHL),Open,"Active High: Output is 3.3V bij aanraken, 0V in rust."
A (AHL),Dicht,"Active Low: Output is 0V bij aanraken, 3.3V in rust."
B (TOG),Open,Direct Mode: Pin is alleen actief zolang je drukt (zoals een deurbel).
B (TOG),Dicht,"Toggle Mode: Eén keer drukken is AAN, nogmaals drukken is UIT (zoals een lichtknop)."
*/
void goToDeepSleep() {
  Serial.println("Deep Sleep configureren...");

  // 1. Zorg dat de pin als input met pull-up staat
  pinMode(5, INPUT_PULLDOWN);

  // 2. Schakel de GPIO-wekker in voor pin 5 (specifiek voor de C3)
  // De C3 heeft deze extra stap nodig om de GPIO laag-niveau trigger te activeren
  gpio_deep_sleep_wakeup_enable(GPIO_NUM_5, GPIO_INTR_HIGH_LEVEL);

  // 3. Vertel de sleep manager om de chip wakker te maken bij een LOW signaal op pin 5
  esp_deep_sleep_enable_gpio_wakeup(1ULL << 5, ESP_GPIO_WAKEUP_GPIO_HIGH);

  // 4. Optioneel: Stel ook een timer in (bijv. 1 uur) zodat hij sowieso af en toe ververst
  esp_sleep_enable_timer_wakeup(getSecondsToSleep(4) * 1000000ULL);

  Serial.println("Welterusten.");
  Serial.flush(); // Zorg dat alle tekst nog naar de computer wordt gestuurd

  // 5. GA SLAPEN
  esp_deep_sleep_start();
}
bool reported = false;
void loop() {
  ArduinoOTA.handle();
  if (!reported) {
    Serial.println("awaiting upload");
    reported = true;
  }
  if (millis() > 1000 * 3600) {
    Serial.println("Going to sleep");
    goToDeepSleep();
  }
}