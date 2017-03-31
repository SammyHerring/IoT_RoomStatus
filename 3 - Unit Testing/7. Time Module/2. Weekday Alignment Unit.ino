//Any earlier values are required for optimal operation, but may be discounted from this test.

/* ================ SETUP CODE (RUNS ONCE) ==================== */

void setup() {

}

/* ================ MAIN CODE (RUN REPEATEDLY) ================= */

void loop() {
  //The main algorithm calls the 'weekdayAlignment()' function at appropriate intervals.
  //This will be tested later in the integration tests.
}

//Realign the day variable to be inline with the reservation and period time Arrays.
void weekdayAlignment() {
  if (day == 1) {
    day = 7;
  } else if (day == 2) {
    day = 1;
  } else if (day == 3) {
    day = 2;
  } else if (day == 4) {
    day = 3;
  } else if (day == 5) {
    day = 4;
  } else if (day == 6) {
    day = 5;
  } else if (day == 7) {
    day = 6;
  }
  return;
}
