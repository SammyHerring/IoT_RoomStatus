/* ======================= LIBRARY IMPORTS ====================== */

#include "application.h"
#include "neopixel.h" //Neopixel library from Adafruit
#include "Adafruit_GFX.h" //Graphics library from Adafruit
#include "Adafruit_SSD1306.h" //OLED library from Adafruit

/* ======================= PRODUCT SETUP ======================= */

PRODUCT_ID(2636);
PRODUCT_VERSION(1);
bool DEBUG_MODE = false;

/* ========================= CONSTANTS ========================= */
/* SEE A LISTING OF ALL VARIABLES ON PAGE 39 */

/* DEFINE VARIABLES */
String formattedTime;
int day;
String formattedDay;
int period;
int movementVal = 0;
int calibrateTime = 10000;
int pirState = LOW;
bool periodNotified = false;
bool morningNotified = false;
bool periodStateChange = false;
//Boolean value that determines whether the notification regarding period reservation change has been sent
bool periodChangeNotified = false;
//Boolean value that ensures that a reservation offer has been made
bool reservationOffered = false;
//Boolean value that turns true when the period timing value have been calculated
bool periodTimeComputed = false;
//Value used to set whether the switch is open or closed
int centreSwitch = 0;

/* NOTIFICATION JSON PAYLOAD STRINGS */
String notifyString1 = "Period ";
String notifyString2;
String notifyString3 = ": ";
String notifyString4;
String notifyData;

/* DEFINE PINOUTS */
int pir = D2; //Set the pin number of the pir sensor input
int centre = D5; //Set the pin number of the centre switch

#define PIXEL_PIN_PIR D4 //Pin for the RGB Status LED
#define PIXEL_COUNT_PIR 12 //Count of the number of RGB LEDs
#define PIXEL_TYPE_PIR SK6812RGBW //RGB LED Type
#define BRIGHTNESS_PIR 50 //Set Brightnesss of RGB LEDs

#define OLED_RESET A5 //Set the OLED Screen Reset Pin

//Set the size parameters of the IoT Boot Logo
#define IoT_Logo_HEIGHT 64
#define IoT_Logo_WIDTH  128

/* ROOM ALLOCATION ARRAYS */
// FORMAT OF ARRAYS: [WEEKDAY][PERIOD]
 int reservation[5][6] = {
   {0, 0, 0, 1, 0, 1} ,   /*  initialisers for row indexed by 0 | MONDAY */
   {1, 0, 1, 0, 0, 0} ,   /*  initialisers for row indexed by 1 | TUESDAY */
   {0, 0, 0, 1, 1, 0} ,   /*  initialisers for row indexed by 3 | WEDNESDAY */
   {1, 0, 0, 0, 0, 1} ,   /*  initialisers for row indexed by 4 | THURSDAY*/
   {0, 0, 0, 0, 0, 1}     /*  initialisers for row indexed by 5 | FRIDAY */
};

int periodStartHr[5][6] = {
   {9, 10, 11, 12, 13, 14} ,   /*  initializers for row indexed by 0 | MONDAY */
   {9, 10, 11, 12, 13, 14} ,   /*  initializers for row indexed by 1 | TUESDAY */
   {8, 9, 10, 11, 13, 14} ,   /*  initializers for row indexed by 3 | WEDNESDAY */
   {9, 10, 11, 12, 13, 14} ,   /*  initializers for row indexed by 4 | THURSDAY*/
   {9, 10, 11, 12, 13, 14}     /*  initializers for row indexed by 5 | FRIDAY */
};

int periodStartMin[5][6] = {
  {5, 0, 10, 5, 40, 35} ,   /*  initializers for row indexed by 0 | MONDAY */
  {5, 0, 10, 5, 40, 35} ,   /*  initializers for row indexed by 1 | TUESDAY */
  {50, 45, 55, 50, 20, 35} ,   /*  initializers for row indexed by 3 | WEDNESDAY */
  {5, 0, 10, 5, 40, 35} ,   /*  initializers for row indexed by 4 | THURSDAY */
  {5, 0, 10, 5, 40, 35}     /*  initializers for row indexed by 5 | FRIDAY */
};

int periodEndHr[5][6] = {
   {10, 10, 12, 13, 14, 15} ,   /*  initializers for row indexed by 0 | MONDAY */
   {10, 10, 12, 13, 14, 15} ,   /*  initializers for row indexed by 1 | TUESDAY */
   {9, 10, 11, 12, 14, 15} ,   /*  initializers for row indexed by 3 | WEDNESDAY */
   {10, 10, 12, 13, 14, 15} ,   /*  initializers for row indexed by 4 | THURSDAY*/
   {10, 10, 12, 13, 14, 15}     /*  initializers for row indexed by 5 | FRIDAY */
};

int periodEndMin[5][6] = {
   {0, 55, 5, 0, 35, 30} ,   /*  initializers for row indexed by 0 | MONDAY */
   {0, 55, 5, 0, 35, 30} ,   /*  initializers for row indexed by 1 | TUESDAY */
   {45, 40, 50, 45, 15, 30} ,   /*  initializers for row indexed by 3 | WEDNESDAY */
   {0, 55, 5, 0, 35, 30} ,   /*  initializers for row indexed by 4 | THURSDAY*/
   {0, 55, 5, 0, 35, 30}     /*  initializers for row indexed by 5 | FRIDAY */
};

//Arrays only declared, where the values are calculated by the algorithm.
int periodStart[5][6];
int periodEnd[5][6];

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

/* ================ SETUP CODE (RUNS ONCE) ==================== */

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

  bootOLED(); //Call the bootOLED() function
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

/* ================ MAIN CODE (RUN REPEATEDLY) ================= */

void loop() {
  updateTime(); //Call updateTime function
  if (Particle.connected()) {Particle.process();}; //Process particle data if connected
  //Continue if connected to Particle Cloud
  while (Particle.connected()) {
    if (!periodTimeComputed) {
      Serial.println("Period Times Not Yet Computed");
      Time.beginDST();
      setPeriodTimes();
      periodTimeComputed = true;
    }
    while (periodTimeComputed) {
      delay(2000);
      updateTime(); //Call updateTime function
      checkMode(); //Call checkMode function
      updateMotion(); //Call updateMotion function

      //Print time values on serial
      Serial.println(formattedTime);
      Serial.println(formattedDay);
      Serial.println(day);
      Serial.println(period);

      delay(5000);
      updateTime(); //Call updateTime function
      delay(5000);

      notifyGoodMorning(); //Call notifyGoodMorning function

      //Checks reservation status and sends appropriate notification.
      //See Logic tables on page 41 for more information.
      if ((1 <= day <= 7) && (0 <= period <= 5)) {

        if (reservation[day][period] == 0) {
          if (pirState == HIGH) {
            //Set the RGB LED Colour to Green
            colorWipe_PIR(pir_neo.Color(0, 150, 0), 0);
            pir_neo.show(); // Initialize all pixels to 'off'
            notify("vacant", "G16 is currently not booked, but movement has been detected.");
          } else {
            //Set the RGB LED Colour to Green
            colorWipe_PIR(pir_neo.Color(0, 150, 0), 0);
            pir_neo.show(); // Initialize all pixels to 'off'
            notify("vacant", "G16 is currently vacant.");
          }
          //If the room is avaliable offer room booking
          //Offer room booking for 2 minutes
          unsigned long startTime = millis();
          if (!periodStateChange && reservationOffered) {
  	        while (millis() - startTime < 120000) {
                display.clearDisplay();
                display.setTextSize(1);
                display.setTextColor(WHITE);
                display.setCursor(0,0);
                display.println("Reservation Avaliable.");
                display.println("Press button to book.");
                display.display();
                delay(1000);
                display.clearDisplay();
                display.display();
                Serial.println("Reservation oppourtunity. Press button.");
                //Wait for the button to be pressed
                if (digitalRead(centre) == LOW) {
                  //If button pressed, set periodStateChange variable to true to notify users
                  //Also set reservationOffered variable to true to prevent the system offering the reservation twice.
                  periodStateChange = true;
                  reservationOffered = true;
                  break;

                }
  	        }
            //Prevent reservation being offered more than once
            reservationOffered = true;
          }
        } else if (reservation[day][period] == 1) {
          //Set the RGB LED Colour to Red
          colorWipe_PIR(pir_neo.Color(150, 0, 0), 0);
          pir_neo.show(); // Initialize all pixels to 'off'
          notify("reserved", "G16 is currently reserved.");
        }
      } else {
        Serial.println("Notification Payload Logic Error.");
        Serial.println("Time Parameters not within standardised region.");
      }
      Particle.process(); //Process Particle Cloud communication data
      break;
    }
  }
}

/* ======================== ALL FUNCTIONS ====================== */

/* NOTIFICATION MODULE */

//Sends daily schedule notification 15 minutes before period one.
//See Logic tables on page 41 for more information.
void notifyGoodMorning() {
  int minSinceMidnight = (Time.local() % 86400) / 60;
  if ( (periodStart[day-1][0]-30) == minSinceMidnight && !morningNotified ) {
    Particle.publish("IoTRoomSensor-GoodMorningIntro-G16", notifyData, PRIVATE);
    delay(3000);
    for (int i=0;i<6;i++) {
      notifyString2 = i+1;
      if ( reservation[day-1][i] == 1 ) {
        notifyString4 = "Booked";
        notifyData = notifyString1+notifyString2+notifyString3+notifyString4;
        Serial.print("Period Notified - Red ");
        Serial.println(i);
        Particle.publish("IoTRoomSensor-GoodMorningBooked-G16", notifyData, PRIVATE);
      } else if (reservation[day-1][i] == 0) {
        notifyString4 = "Available";
        notifyData = notifyString1+notifyString2+notifyString3+notifyString4;
        Serial.print("Period Notified - Green: ");
        Serial.println(i);
        Particle.publish("IoTRoomSensor-GoodMorningAvailable-G16", notifyData, PRIVATE);
      } else {
        Serial.print("Error on: ");
        Serial.println(i);
      }
      delay(3000);
    }
    Particle.publish("IoTRoomSensor-GoodMorningTS-G16", "", PRIVATE);
    morningNotified = true;
    delay(5000);
    return;
  } else if (periodEnd[day-1][5] == minSinceMidnight) {
    morningNotified = false;
  }
}

//Send general notifications to the correct Slack Webhook using the Particle Cloud with JSON DATA.
//See Logic tables on page 41 for more information.
void notify(char type[], char data[]) {

  if ( type == "boot" ) {
    Particle.publish("IoTRoomSensor-Boot-G16", data, PRIVATE);
  } else if ( !periodNotified || periodStateChange ) {
      if (periodStateChange && !periodChangeNotified) {
        Serial.println("Notification: New Reservation");
        Particle.publish("IoTRoomSensor-StateRed-G16", "G16 has been reserved.", PRIVATE);
        periodChangeNotified = true;
      } else if ( type == "reserved" ) {
        Serial.println("Notification: Reserved");
        Serial.println(periodNotified);
        Serial.println(periodStateChange);
        Particle.publish("IoTRoomSensor-StateRed-G16", data, PRIVATE);
        periodNotified = true;
      } else if ( type == "vacant" ) {
        Serial.println("Notification: Vacant");
        Serial.println(periodNotified);
        Serial.println(periodStateChange);
        Particle.publish("IoTRoomSensor-StateGreen-G16", data, PRIVATE);
        periodNotified = true;
      }
    } else {
      Serial.println("Notification Pushed. Type unknown.");
      Serial.println(type);
      Serial.println(data);
    }
}

/* TIME MODULE */

//Update all time variables
void updateTime() {
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

//Realign the day variable to be inline with the reservation and period time Arrays.
//See Logic tables on page 42 for more information.
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

/* PERIOD VARIABLE CONTROL (A PART OF TIME MODULE) */
//Return the current period value by using the periodStart and periodEnd arrays.
//See Logic tables on page 42 for more information.
int updatePeriod() {

    int minSinceMidnight = (Time.local() % 86400) / 60;
    for (int i=0;i<6;i++) {
      if (day == 6 || day == 7 ) {
        return 7;
      } else if ( minSinceMidnight < (periodStart[day][0]+30) ) {
        return 8;
      } else if ( minSinceMidnight > (periodEnd[day][5]+30) ) {
        return 9;
      } else if ( periodStart[day][i] <= minSinceMidnight && minSinceMidnight < periodEnd[day][i] ) {
        if ( period != 0 && i != period ) {
          periodNotified = false;
        } else {
          periodNotified = true;
        }
        return i;
      }
    }
}

//Set time values (minutes) for the periodStart and periodEnd arrays.
//See Logic tables on page 42 for more information.
void setPeriodTimes() {
  for (int x=0; x<5; x++) {
    for (int y=0; y<6; y++) {
      int timeConvertStart = (periodStartHr[x][y] * 60) + periodStartMin[x][y];
      int timeConvertEnd = (periodEndHr[x][y] * 60) + periodEndMin[x][y];
      periodStart[x][y] = timeConvertStart;
      periodEnd[x][y] = timeConvertEnd;
      if (DEBUG_MODE) {
        Serial.print("Day: ");
        Serial.print(x+1);
        Serial.print(" | Period: ");
        Serial.print(y+1);
        Serial.print(" | Start Time: ");
        Serial.print(periodStart[x][y]);
        Serial.print(" | End Time: ");
        Serial.println(periodEnd[x][y]);
    }
    }
  }
  return;
}

/* OFFLINE MODE MODULE */

//Check the current period value to see if it is an out of hours operation code and act appropriately.
//In these cases the out of hours operation code leads to calling the offlineMode function.
//See Logic tables on page 42 for more information.
void checkMode() {
  if (period == 7) { //Not weekday
    offlineMode(7, "Not a Weekday");
    System.reset();
  } else if (period == 8) { //Period 1 more than 30 minutes away
    offlineMode(8, "Period 1 > 30 Minutes Away");
    System.reset();
  } else if (period == 9) { //Period 6 more than 30 minutes ago
    offlineMode(9, "Period 6 > 30 Minutes Ago");
    System.reset();
  } else if (period < 1 || period > 6) {
    Serial.print("Non-standard Period Detected. Period: ");
    Serial.println(period);
  }
  return;
}

//Place the system in a sleep mode until the system is due to come into operation.
void offlineMode(int periodMode, char reason[]) {
  //Print the offline mode and reasoning on serial
  Serial.print("--Offline Mode - ");
  Serial.print(periodMode);
  Serial.println("--");
  Serial.println(reason);
  //If in offline mode show white colour on RGB Status LEDs
  fullWhite();
  //Notify the users of Offline Mode on the OLED Display
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Offline");
  display.display();
  delay(1500);
  //While the period is an out of hours operation code
  while (period == periodMode) {
    //pulseWhite(5);
    Particle.process(); //Process Particle Cloud communication data
    period = updatePeriod(); //Update the period variable value
    System.sleep(300); //Enter sleep mode for 300 seconds, where the system is in a low power state.
  }
  return;
}

/* MOTION DETECTION MODULE */

//Read and push the PIR values to the notify function
void updateMotion() {
    if (calibrated()) { //Prior to processing the PIR data, ensure the sensor has had time to calibrate.
      readPIR(); //Call the readPIR function
      reportPIR(); //Call the reportPIR function
    }
  return;
}

//Calculate whether an appropriate calibration period has elapsed based on system run time.
bool calibrated() {
  return millis() - calibrateTime > 0; //If the appropriate time has elapsed, give a 'true' boolean value.
}

//Set the PIR movement value
void readPIR() {
  movementVal = digitalRead(pir); //Set the movementVal variable based on the digital value of the 'pir' pin.
}

//Report the PIR movement state based on the movementVal variable.
//See Logic tables on page 42 for more information.
void reportPIR() {
  if (movementVal == HIGH) {
    if (pirState == LOW) {
      Serial.println("Motion Detected.");
      pirState = HIGH;
    }
  } else {
    if (pirState == HIGH) {
      pirState = LOW;
    }
  }
}

/* PIR RGB LED MODULE */

// Fill each pixel one after the other with a given color
void colorWipe_PIR(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<pir_neo.numPixels(); i++) {
    pir_neo.setPixelColor(i, c);
    pir_neo.show();
    delay(wait);
  }
}

//Set the RGB LEDs to full white
void fullWhite() {
  for(uint16_t i=0; i<pir_neo.numPixels(); i++) {
    pir_neo.setPixelColor(i, pir_neo.Color(0,0,0, 255 ) );
  }
  pir_neo.show();
}

/* DISPLAY INFORMATION MODULE */

//Wi-Fi Connecting animation
void loadingOLED() {
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print(" ");
  display.println("Wi-Fi");
  display.setTextSize(1);
  display.println("");
  display.setTextSize(3);
  display.println("  <->");
  display.display();
  display.clearDisplay();
  display.setCursor(0,0);
  display.print(" ");
  display.println("Wi-Fi");
  display.setTextSize(1);
  display.println("");
  display.setTextSize(3);
  display.println(" <--->");
  display.display();
  display.clearDisplay();
  display.setCursor(0,0);
  display.print(" ");
  display.println("Wi-Fi");
  display.setTextSize(1);
  display.println("");
  display.setTextSize(3);
  display.println("<----->");
  display.display();
  display.clearDisplay();
  display.setCursor(0,0);
  display.print(" ");
  display.println("Wi-Fi");
  display.setTextSize(1);
  display.println("");
  display.setTextSize(3);
  display.println(" <--->");
  display.display();
}

//Notification that the system is ready on the OLED
void readyOLED() {
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Ready.");
  display.display();
  delay(1000);
  display.clearDisplay();
  display.display();
}

//Test OLED functionality at boot
void bootOLED(void) {
  display.clearDisplay();
  display.display();
  for (int16_t i=0; i<min(display.width(),display.height())/2; i+=5) {
    display.drawTriangle(display.width()/2, display.height()/2-i,
                     display.width()/2-i, display.height()/2+i,
                     display.width()/2+i, display.height()/2+i, WHITE);
    display.display();
  }
  uint8_t color = WHITE;
  for (int16_t i=min(display.width(),display.height())/2; i>0; i-=5) {
    display.fillTriangle(display.width()/2, display.height()/2-i,
                     display.width()/2-i, display.height()/2+i,
                     display.width()/2+i, display.height()/2+i, WHITE);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    display.display();
  }
}
