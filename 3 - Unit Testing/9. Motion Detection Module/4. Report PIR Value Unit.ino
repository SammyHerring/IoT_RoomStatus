//Any earlier values are required for optimal operation, but may be discounted from this test.

/* ================ SETUP CODE (RUNS ONCE) ==================== */

void setup() {

}

/* ================ MAIN CODE (RUN REPEATEDLY) ================= */

void loop() {
  //The main algorithm calls the 'readPIR()' function at appropriate intervals.
  //This will be tested later in the integration tests.
}

//Report the PIR movement state based on the movementVal variable.
void reportPIR() {
  if (movementVal == HIGH) {
    if (pirState == LOW) {
      pirState = HIGH;
    }
  } else {
    if (pirState == HIGH) {
      pirState = LOW;
    }
  }
}
