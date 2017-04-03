/* ======================= LIBRARY IMPORTS ====================== */

#include "application.h"

/* ======================= PRODUCT SETUP ======================= */

PRODUCT_ID(2636);
PRODUCT_VERSION(1);
bool DEBUG_MODE = false;

/* ========================= CONSTANTS ========================= */

/* DEFINE VARIABLES */
String formattedTime;
int day;
String formattedDay;
int period = 0;
int movementVal = 0;
int calibrateTime = 10000;
int pirState = LOW;
bool periodNotified = false;
bool morningNotified = false;
bool periodStateChange = false;

/* NOTIFICATION JSON PAYLOAD STRINGS */
String notifyString1 = "Period ";
String notifyString2;
String notifyString3 = ": ";
String notifyString4;
String notifyData;

/* DEFINE PINOUTS */
int pir = D2;

/* ROOM ALLOCATION ARRAYS */
// FORMAT OF ARRAYS: [WEEKDAY][PERIOD]
 int reservation[5][6] = {
   {1, 1, 0, 1, 1, 1} ,   /*  initialisers for row indexed by 0 | MONDAY */
   {1, 0, 1, 1, 0, 1} ,   /*  initialisers for row indexed by 1 | TUESDAY */
   {1, 1, 0, 1, 1, 0} ,   /*  initialisers for row indexed by 3 | WEDNESDAY */
   {1, 0, 1, 0, 1, 1} ,   /*  initialisers for row indexed by 4 | THURSDAY*/
   {0, 1, 1, 1, 1, 1}     /*  initialisers for row indexed by 5 | FRIDAY */
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

/* ================ SETUP CODE (RUNS ONCE) ==================== */

void setup() {
  Serial.begin(115200); //Open Serial Transmission Port at BAUD Rate of 115200

  pinMode(pir, INPUT); //Set pinMode of D2 (pir) to INPUT

  //If DEBUG_MODE 'true' then enable on-board Status LED else disable on-board Status LED.
  if (!DEBUG_MODE) {
    RGB.control(true);
    RGB.color(0, 0, 0);
  } else {
    RGB.control(false);
  }

  WiFi.connect(WIFI_CONNECT_SKIP_LISTEN); //Connect to Wi-Fi network, regardless of network signal.
  int DBG = 0;
  while (!WiFi.ready()) {
    //Print Wi-Fi not connected yet once to serial if Wi-Fi not ready
    if (DBG == 0) {Serial.println("Wi-Fi Antenna Not Ready."); DBG = 1;}
    } while (WiFi.ready()) {
    Serial.println("Wi-Fi Antenna Ready.");
    Serial.println("W-Fi Connecting.");
    Particle.connect();
    //Continue once connected to Particle Cloud
    while (Particle.connected()) {
      Serial.println("W-Fi Connected."); //Print connected to Wi-Fi on serial
      Particle.syncTime(); //Sync RTC to Particle Cloud time value
      setPeriodTimes(); //Call setPeriodTimes function
      delay(2000);
      updateTime(); //Call updateTime function
      //Print time values on serial
      Serial.println(formattedTime);
      Serial.println(formattedDay);
      Serial.println(day);
      Serial.println(period);
      Serial.println("Setup Complete."); //Print Setup complete on serial
      break;
    }
    break;
  }
}

/* ================ MAIN CODE (RUN REPEATEDLY) ================= */

void loop() {
  updateTime(); //Call updateTime function
  if (Particle.connected()) {Particle.process();}; //Process particle data if connected
  //Continue if connected to Particle Cloud
  while (Particle.connected()) {
    delay(2000);
    updateTime(); //Call updateTime function

    //Print time values on serial
    Serial.println(formattedTime);
    Serial.println(formattedDay);
    Serial.println(day);
    Serial.println(period);

    delay(5000);
    updateTime(); //Call updateTime function
    delay(5000);

    //Checks reservation status and sends appropriate notification.
    if ((1 <= day <= 7) && (0 <= period <= 5)) {
      if (reservation[day-1][period] == 0) {
        //Notification Module not implemented, therefore tested using Serial output.
        Serial.println("Vacant. No movement detected.");
      } else if (reservation[day-1][period] == 1) {
        //Notification Module not implemented, therefore tested using Serial output.
        Serial.println("Reserved.");
      }
    } else {
      Serial.println("Notification Payload Logic Error.");
      Serial.println("Time Parameters not within standardised region.");
    }
    Particle.process(); //Process Particle Cloud communication data
    break;
  }
}

/* ======================== ALL FUNCTIONS ====================== */

/* TIME MODULE */

//Update all time variables
void updateTime() {
  formattedDay = Time.format(Time.now(), "%A");
  formattedTime = Time.format(Time.now(), "%I:%M%p");
  day = Time.weekday();
  weekdayAlignment(); //Call weekdayAlignment function
  period = updatePeriod();
  return;
}

//Realign the day variable to be inline with the reservation and period time Arrays.
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
int updatePeriod() {

    int minSinceMidnight = (Time.local() % 86400) / 60;
    for (int i=0;i<6;i++) {
      if (day == 6 || day == 7 ) {
        return 7;
      } else if ( minSinceMidnight < (periodStart[day-1][0]+30) ) {
        return 8;
      } else if ( minSinceMidnight > (periodEnd[day-1][5]+30) ) {
        return 9;
      } else if ( periodStart[day-1][i] <= minSinceMidnight && minSinceMidnight < periodEnd[day-1][i] ) {
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
