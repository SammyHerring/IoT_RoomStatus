//Any earlier values are required for optimal operation, but may be discounted from this test.

/* ================ SETUP CODE (RUNS ONCE) ==================== */

void setup() {

}

/* ================ MAIN CODE (RUN REPEATEDLY) ================= */

void loop() {
  //The main algorithm calls the 'calibrated()' function at appropriate intervals.
  //This will be tested later in the integration tests.
}

//Calculate whether an appropriate calibration period has elapsed based on system run time.
bool calibrated() {
  return millis() - calibrateTime > 0; //If the appropriate time has elapsed, give a 'true' boolean value.
}
