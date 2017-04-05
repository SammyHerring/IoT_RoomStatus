void setup() {
  Serial.begin(115200); //Open Serial Transmission Port at BAUD Rate of 115200

  pinMode(pir, INPUT); //Set pinMode of D2 (pir) to INPUT
  pinMode(centre, INPUT_PULLUP); //Set pinMode of D5 Centre Switch to INPUT PULLED HIGH

  //Set the RGB Status LED to be blank
  pir_neo.setBrightness(BRIGHTNESS_PIR);
  pir_neo.begin();
  colorWipe_PIR(pir_neo.Color(0, 0, 0), 0);
  pir_neo.show(); // Initialize all pixels to 'off'

  //Start the display serial transmission and clear the display's content
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.display();
  delay(3000);
  display.clearDisplay();   // clears the screen and buffer

  delay(500);

  //If DEBUG_MODE 'true' then enable on-board Status LED else disable on-board Status LED.
  //See Logic tables on page 41 for more information.
  if (!DEBUG_MODE) {
    RGB.control(true);
    RGB.color(0, 0, 0);
  } else {
    RGB.control(false);
  }

  WiFi.connect(WIFI_CONNECT_SKIP_LISTEN); //Connect to Wi-Fi network, regardless of network signal.
  int DBG = 0;
  while (!WiFi.ready()) {
    loadingOLED(); //Call the loadingOLED function()
    //Print Wi-Fi not connected yet once to serial if Wi-Fi not ready
    if (DBG == 0) {Serial.println("Wi-Fi Antenna Not Ready."); DBG = 1;}
  } while (WiFi.ready()) {
    Serial.println("Wi-Fi Antenna Ready.");
    Serial.println("W-Fi Connecting.");
    Particle.connect();
    Serial.println("W-Fi Connected."); //Print connected to Wi-Fi on serial
    Particle.syncTime(); //Sync RTC to Particle Cloud time value
    Time.beginDST();
    notify("boot", "IoT Sensor now online."); //Call notify function with boot data
    readyOLED(); //Call the readyOLED() function
    Serial.println("Setup Complete."); //Print Setup complete on serial
    break;
  }
}
