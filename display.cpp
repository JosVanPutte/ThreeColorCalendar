#include "google.h"
#include "icons.h"

#define INFO Serial.printf

#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold9pt7b.h>

// 1. PIN DEFINITIES (Controleer of je draden hier zitten!)
#define EPD_SCL   4  
#define EPD_SDA   3  
#define EPD_CS    0  
#define EPD_DC    1  
#define EPD_RES   2  
#define EPD_BUSY  7
#define WAKEUP_PIN 5 // Jouw fysieke knop

// 2. CONSTRUCTOR (Geoptimaliseerd voor de beperkte RAM van de C3)
// We gebruiken '64' als paginahoogte om RAM-crashes te voorkomen.
GxEPD2_3C<GxEPD2_290_C90c, 296> display(GxEPD2_290_C90c(EPD_CS, EPD_DC, EPD_RES, EPD_BUSY));

extern const char *getIconFound();

void drawWeatherIcon(int x, int y, bool crashed) {
  bool found = false;
  const char *icon = getIconFound();
  INFO("draw weather Icon %s\n", icon);
  for (int i = 0; i < iconCount && !crashed; i++) {
    if (!strcmp(icon, weather_map[i].id)) {
      // Teken de zwarte laag
      if (weather_map[i].black_data != NULL) {
        display.drawBitmap(x, y, weather_map[i].black_data, 64, 64, GxEPD_BLACK);
      }
      
      // Teken de rode laag (als die bestaat)
      if (weather_map[i].red_data != NULL) {
        display.drawBitmap(x, y, weather_map[i].red_data, 64, 64, GxEPD_RED);
      }
      
      found = true;
      break;
    }
  }

  if (!found) {
    if (!crashed) {
      display.drawBitmap(x, y, epd_bitmap_unknown, 64, 64, GxEPD_BLACK);
    }
    display.setCursor(x, 121);
    display.print(String(icon) + " ??");
  }
}
void setupDisplay(struct calendarEntries *entries, const char * w, bool crashed) {
  INFO("Display initialisatie met %d entries!\n",calEntryCount);
  pinMode(EPD_CS, OUTPUT);
  pinMode(EPD_DC, OUTPUT);
  pinMode(EPD_RES, OUTPUT);
  pinMode(EPD_BUSY, INPUT);

  // 1. Initialiseer E-paper op C3 Hardware SPI
  SPI.begin(EPD_SCL, -1, EPD_SDA, EPD_CS);
  display.init(115200, true, 2, false);
  INFO("Display geinitialiseerd %d x %d !\n", display.width(), display.height());

  // 4. TEKENEN (De "Paged" methode)
  display.setRotation(1); // Landscape modus
  display.setTextSize(1);
  display.setFullWindow();

  display.firstPage();
  int y = 12;
  do {
    display.fillScreen(GxEPD_WHITE);
    
    int ntry = 0;
    // Zwarte tekst
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(0, y);
    display.print(entries[ntry].calTime.substring(0,10));
    y += 20;
    for (ntry = 0; ntry < calEntryCount && y < 121 - 8; ++ntry) {
       display.setCursor(10,y);
       String line = entries[ntry].calTime.substring(16,21) + String(" ") + entries[ntry].calTitle;
       display.print(line.substring(0,46));
       y+=16;
    }

    drawWeatherIcon(296 - 64, 128 - 64, crashed);

    // Rode tekst (SSD1680 ondersteunt dit)
    display.setTextColor(GxEPD_RED);
    display.setCursor(0, 127);
    display.print(w);

  } while (display.nextPage());

  Serial.println("Tekenen voltooid.");
  display.hibernate();
}