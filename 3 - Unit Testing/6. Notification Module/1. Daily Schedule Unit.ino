//Any earlier values are required for optimal operation, but may be discounted from this test.

/* ================ SETUP CODE (RUNS ONCE) ==================== */

void setup() {

}

/* ================ MAIN CODE (RUN REPEATEDLY) ================= */

void loop() {
  //The main algorithm calls the 'notifyGoodMorning()' function at appropriate intervals.
  //This will be tested later in the integration tests.
}

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
