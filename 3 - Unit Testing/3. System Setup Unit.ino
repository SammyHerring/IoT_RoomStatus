/* ======================= SYSTEM SETUP ======================== */

STARTUP(
  WiFi.selectAntenna(ANT_AUTO); //Allows the device to choose the antenna with the best signal.
  System.enableUpdates(); //Allows the device to download OTA updates at boot.
  );
SYSTEM_MODE(MANUAL); //Forces the system to wait for cloud processing until allowed by algorithm.
SYSTEM_THREAD(ENABLED); //Allows a secondary thread to be used for the cloud processing.

//Setup the parameters of the RGB and OLED functions
Adafruit_NeoPixel pir_neo = Adafruit_NeoPixel(PIXEL_COUNT_PIR, PIXEL_PIN_PIR, PIXEL_TYPE_PIR);
Adafruit_SSD1306 display(OLED_RESET);

//The setup and loop components added below to allow the file to successfully compile.
//However, this unit does not run within them as they are apart of the system setup.

/* ================ SETUP CODE (RUNS ONCE) ==================== */

void setup() {

}

/* ================ MAIN CODE (RUN REPEATEDLY) ================= */

void loop() {

}
