# ThreeColorCalendar
<img src="https://github.com/JosVanPutte/ThreeColorCalendar/blob/main/version%200.1.jpg" alt="version 0.1" >
<img src="https://github.com/JosVanPutte/ThreeColorCalendar/blob/main/wired.jpg" alt="wired" >

This Arduino IDE sketch is to use a 2.9 inch 3 color (BWR) epaper that shows my calendar and the weather for the day.<BR>I use OTA for this sketch as it runs on a ESP32C3 that I found hard to program by wire.<BR>The google script to use to get the calendar items from google is https://github.com/JosVanPutte/EpaperCalendar/blob/main/googleScript.gs<BR>
The battery is 900 mah and the HT7333 voltage regulator is very efficient. A TP4056 charger board is used.<BR> A TTP223 button in the case wakes up the ESP32C3 (on GPIO 5) so it starts listening for OTA updates for 15 minutes.
