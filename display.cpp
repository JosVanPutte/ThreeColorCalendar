void INFO(const char*s, const char* m);

#define DISPLAY_USED_FOR_JOS
#ifdef DISPLAY_USED_FOR_JOS
#include <GxEPD2_3C.h>

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
GxEPD2_3C<GxEPD2_290_C90c, 64> display(GxEPD2_290_C90c(EPD_CS, EPD_DC, EPD_RES, EPD_BUSY));

void setupDisplay(const char * w) {
  INFO("Display initialisatie!\n","");
  pinMode(EPD_CS, OUTPUT);
  pinMode(EPD_DC, OUTPUT);
  pinMode(EPD_RES, OUTPUT);
  pinMode(EPD_BUSY, INPUT);

  // 1. Initialiseer E-paper op C3 Hardware SPI
  SPI.begin(EPD_SCL, -1, EPD_SDA, EPD_CS);
  display.init(115200, true, 2, false);
  INFO("Display geinitialiseerd!\n","");

  // 4. TEKENEN (De "Paged" methode)
  display.setRotation(1); // Landscape modus
  display.setFont();
  display.setTextSize(1);
  display.setFullWindow();

  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    
    // Zwarte tekst
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(10, 30);
    display.print("Hello Jos.");

    // Rode tekst (SSD1680 ondersteunt dit)
    display.setTextColor(GxEPD_RED);
    display.setCursor(10, 70);
    display.print("ESP32-C3 Online");

    display.setCursor(0, 121);
    display.print(w);

  } while (display.nextPage());

  Serial.println("Tekenen voltooid.");
  display.hibernate();
}
#else
void setupDisplay() {
    INFO("Geen display geinitialiseerd!\n","");
}
#endif
