#pragma once
#include "globals.h"

inline const char* get_ordinal(int day) {
  if (day == 1 || day == 21 || day == 31) return "ST";
  if (day == 2 || day == 22)              return "ND";
  if (day == 3 || day == 23)              return "RD";
  return "TH";
}

inline const char* get_month_str() {
  int m = now.month();
  const char* months[] = {
    "", "JANUARY", "FEBRUARY", "MARCH", "APRIL", "MAY", "JUNE",
    "JULY", "AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER", "DECEMBER"
  };
  if (m >= 1 && m <= 12) return months[m];
  return "";
}

inline const char* get_day_str() {
  int d = now.dayOfTheWeek();
  const char* days[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
  if (d >= 0 && d <= 6) return days[d];
  return "";
}
