//Any earlier values are required for optimal operation, but may be discounted from this test.

/* ================ SETUP CODE (RUNS ONCE) ==================== */

void setup() {

}

/* ================ MAIN CODE (RUN REPEATEDLY) ================= */

void loop() {
  //The main algorithm calls the 'checkMode()' function at appropriate intervals.
  //This will be tested later in the integration tests.
}

//Check the current period value to see if it is an out of hours operation code and act appropriately.
//In these cases the out of hours operation code leads to calling the offlineMode function.
void checkMode() {
  if (period == 7) { //Not weekday
    offlineMode(7, "Not a Weekday");
    System.reset();
  } else if (period == 8) { //Period 1 more than 30 minutes away
    offlineMode(8, "Period 1 > 30 Minutes Away");
    System.reset();
  } else if (period == 9) { //Period 6 more than 30 minutes ago
    offlineMode(9, "Period 6 > 30 Minutes Ago");
    System.reset();
  }
  return;
}
