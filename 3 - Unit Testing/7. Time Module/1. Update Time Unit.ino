//Any earlier values are required for optimal operation, but may be discounted from this test.

/* ================ SETUP CODE (RUNS ONCE) ==================== */

void setup() {

}

/* ================ MAIN CODE (RUN REPEATEDLY) ================= */

void loop() {
  //The main algorithm calls the 'updateTime()' function at appropriate intervals.
  //This will be tested later in the integration tests.
}

if (periodTimeComputed) {
  int minSinceMidnight = (Time.local() % 86400) / 60;
  formattedDay = Time.format(Time.now(), "%A"); //See variables listing on page 39.
  formattedTime = Time.format(Time.now(), "%I:%M%p"); //See variables listing on page 39.
  day = Time.weekday(); //See variables listing on page 39.
  weekdayAlignment(); //Call weekdayAlignment function
  period = updatePeriod(); //See variables listing on page 39.

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(formattedDay);
  display.println(formattedTime);
  display.display();

  //Reset the booking reservation variables for the next period
  if (minSinceMidnight == periodEnd[day][period]) {
    periodStateChange = false;
    periodChangeNotified = false;
    reservationOffered = false;
  }
  return;
}
}
