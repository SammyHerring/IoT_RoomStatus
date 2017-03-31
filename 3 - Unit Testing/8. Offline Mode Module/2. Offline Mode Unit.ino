//Any earlier values are required for optimal operation, but may be discounted from this test.

/* ================ SETUP CODE (RUNS ONCE) ==================== */

void setup() {

}

/* ================ MAIN CODE (RUN REPEATEDLY) ================= */

void loop() {
  //The main algorithm calls the 'offlineMode()' function at appropriate intervals.
  //This will be tested later in the integration tests.
}

//Place the system in a sleep mode until the system is due to come into operation.
void offlineMode(int periodMode, char reason[]) {
  //Print the offline mode and reasoning on serial
  Serial.print("--Offline Mode - ");
  Serial.print(periodMode);
  Serial.println("--");
  Serial.println(reason);
  delay(1500);
  //While the period is an out of hours operation code
  while (period == periodMode) {
    Particle.process(); //Process Particle Cloud communication data
    period = updatePeriod(); //Update the period variable value
    System.sleep(300); //Enter sleep mode for 300 seconds, where the system is in a low power state.
  }
  return;
}
