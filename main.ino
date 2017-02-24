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

int periodStart[5][6];
int periodEnd[5][6];

/* ======================= SYSTEM SETUP ======================== */

STARTUP(
  WiFi.selectAntenna(ANT_AUTO);
  System.enableUpdates();
  );
SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);

/* ================ SETUP CODE (RUNS ONCE) ==================== */

void setup() {
  Serial.begin(115200);

  pinMode(pir, INPUT);

  if (!DEBUG_MODE) {
    RGB.control(true);
    RGB.color(0, 0, 0);
  } else {
    RGB.control(false);
  }

  WiFi.connect(WIFI_CONNECT_SKIP_LISTEN);
  int DBG = 0;
  while (!WiFi.ready()) {
    if (DBG == 0) {Serial.println("Wi-Fi Antenna Not Ready."); DBG = 1;}
    } while (WiFi.ready()) {
    Serial.println("Wi-Fi Antenna Ready.");
    Serial.println("W-Fi Connecting.");
    Particle.connect();
    while (!Particle.connected()) {
    } while (Particle.connected()) {
        Serial.println("W-Fi Connected.");
        Particle.syncTime();
        setPeriodTimes();
        delay(2000);
        updateTime();
        Serial.println(formattedTime);
        Serial.println(formattedDay);
        Serial.println(period);
        notify("boot", "IoT Sensor now online.");
        Serial.println("Setup Complete.");
    break;
    }
    break;
  }
}

/* ================ MAIN CODE (RUN REPEATEDLY) ================= */

void loop() {
  updateTime();
  if (Particle.connected()) {Particle.process();};
  while (Particle.connected()) {
    delay(2000);
    updateTime();
    checkMode();
    updateMotion();

    Serial.println(formattedTime);
    Serial.println(formattedDay);
    Serial.println(period);

    delay(5000);
    updateTime();
    delay(5000);

    notifyGoodMorning();

    if (reservation[day-1][period] == 0) {
      if (pirState == HIGH) {
        notify("vacant", "G16 is currently not booked, but movement has been detected.");
      } else {
        notify("vacant", "G16 is currently vacant.");
      }
    } else if (reservation[day-1][period] == 1) {
      notify("reserved", "G16 is currently reserved.");
    }
    Particle.process();
    break;
  }
}

/* ======================== ALL FUNCTIONS ====================== */

/* NOTIFYER FUNCTIONS */

void notifyGoodMorning() {
  int minSinceMidnight = (Time.local() % 86400) / 60;
  if ( (periodStart[day-1][0]-15) == minSinceMidnight && !morningNotified ) {
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

void notify(char type[], char data[]) {

  if ( type == "boot" ) {
    Particle.publish("IoTRoomSensor-Boot-G16", data, PRIVATE);
  }

  if ( !periodNotified || periodStateChange ) {
    if ( type == "reserved" ) {
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
}
  return;
}

/* TIME UPDATE FUNCTIONS */

void updateTime() {
  formattedDay = Time.format(Time.now(), "%A");
  formattedTime = Time.format(Time.now(), "%I:%M%p");
  day = Time.weekday();
  weekdayAlignment();
  period = updatePeriod();
  return;
}

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

/* PERIOD CONTROL FUNCTIONS */

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

/* OFFLINE MODE DURING NON-WEEKDAYS */

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
  }
  return;
}

void offlineMode(int periodMode, char reason[]) {
  Serial.print("--Offline Mode - ");
  Serial.print(periodMode);
  Serial.println("--");
  Serial.println(reason);
  delay(1500);
  while (period == periodMode) {
    Particle.process();
    period = updatePeriod();
    System.sleep(300);
  }
  return;
}

/* PIR SESNOR FUNCTIONS */

void updateMotion() {
    if (calibrated()) {
      readPIR();
      reportPIR();
    }
  return;
}

bool calibrated() {
  return millis() - calibrateTime > 0;
}

void readPIR() {
  movementVal = digitalRead(pir);
}

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
