//Any earlier values are required for optimal operation, but may be discounted from this test.

/* ================ SETUP CODE (RUNS ONCE) ==================== */

void setup() {

}

/* ================ MAIN CODE (RUN REPEATEDLY) ================= */

void loop() {
  //The main algorithm calls the 'setPeriodTimes()' function at appropriate intervals.
  //This will be tested later in the integration tests.
}

//Set time values (minutes) for the periodStart and periodEnd arrays.
void setPeriodTimes() {
  for (int x=0; x<5; x++) {
    for (int y=0; y<6; y++) {
      int timeConvertStart = (periodStartHr[x][y] * 60) + periodStartMin[x][y];
      int timeConvertEnd = (periodEndHr[x][y] * 60) + periodEndMin[x][y];
      periodStart[x][y] = timeConvertStart;
      periodEnd[x][y] = timeConvertEnd;
      if (DEBUG_MODE) {
        Serial.print("Day: ");
        Serial.print(x+1);
        Serial.print(" | Period: ");
        Serial.print(y+1);
        Serial.print(" | Start Time: ");
        Serial.print(periodStart[x][y]);
        Serial.print(" | End Time: ");
        Serial.println(periodEnd[x][y]);
    }
    }
  }
  return;
}
