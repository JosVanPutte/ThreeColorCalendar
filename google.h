#include <Arduino.h>
#include "time.h"

struct calendarEntries
{
  String calTime;  //Format is "Wed Feb 10 2020 10:00"
  String calTitle;
};

extern int calEntryCount;

struct calendarEntries *getCalendar(String prefix);
