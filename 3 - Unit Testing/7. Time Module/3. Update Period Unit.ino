//Any earlier values are required for optimal operation, but may be discounted from this test.

/* ================ SETUP CODE (RUNS ONCE) ==================== */

void setup() {

}

/* ================ MAIN CODE (RUN REPEATEDLY) ================= */

void loop() {
  //The main algorithm calls the 'updatePeriod()' function at appropriate intervals.
  //This will be tested later in the integration tests.
}

//Return the current period value by using the periodStart and periodEnd arrays.
int updatePeriod() {

    int minSinceMidnight = (Time.local() % 86400) / 60;
    for (int i=0;i<6;i++) {
      if (day == 6 || day == 7 ) {
        return 7;
      } else if ( minSinceMidnight < (periodStart[day-1][0]+30) ) {
        return 8;
      } else if ( minSinceMidnight > (periodEnd[day-1][5]+30) ) {
        return 9;
      } else if ( periodStart[day-1][i] <= minSinceMidnight && minSinceMidnight < periodEnd[day-1][i] ) {
        if ( period != 0 && i != period ) {
          periodNotified = false;
        } else {
          periodNotified = true;
        }
        return i;
      }
    }
}
