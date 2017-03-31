//Any earlier values are required for optimal operation, but may be discounted from this test.

/* ================ SETUP CODE (RUNS ONCE) ==================== */

void setup() {

}

/* ================ MAIN CODE (RUN REPEATEDLY) ================= */

void loop() {
  //The main algorithm calls the 'updateTime()' function at appropriate intervals.
  //This will be tested later in the integration tests.
}

void updateTime() {
  formattedDay = Time.format(Time.now(), "%A");
  formattedTime = Time.format(Time.now(), "%I:%M%p");
  day = Time.weekday();
  weekdayAlignment(); //Call weekdayAlignment function
  period = updatePeriod();
  return;
}
