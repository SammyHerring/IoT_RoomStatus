//Any earlier values are required for optimal operation, but may be discounted from this test.

/* ================ SETUP CODE (RUNS ONCE) ==================== */

void setup() {

}

/* ================ MAIN CODE (RUN REPEATEDLY) ================= */

void loop() {
  //The main algorithm calls the 'updateMotion()' function at appropriate intervals.
  //This will be tested later in the integration tests.
}

//Read and push the PIR values to the notify function
void updateMotion() {
    if (calibrated()) { //Prior to processing the PIR data, ensure the sensor has had time to calibrate.
      readPIR(); //Call the readPIR function
      reportPIR(); //Call the reportPIR function
    }
  return;
}
