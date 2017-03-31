//Any earlier values are required for optimal operation, but may be discounted from this test.

/* ================ SETUP CODE (RUNS ONCE) ==================== */

void setup() {

}

/* ================ MAIN CODE (RUN REPEATEDLY) ================= */

void loop() {
  //The main algorithm calls the 'readPIR()' function at appropriate intervals.
  //This will be tested later in the integration tests.
}

//Set the PIR movement value
void readPIR() {
  movementVal = digitalRead(pir); //Set the movementVal variable based on the digital value of the 'pir' pin.
}
