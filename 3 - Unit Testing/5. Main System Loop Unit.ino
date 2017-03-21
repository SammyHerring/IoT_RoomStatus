//Any earlier values are required for optimal operation, but may be discounted from this test.

/* ================ SETUP CODE (RUNS ONCE) ==================== */

void setup() {

}

/* ================ MAIN CODE (RUN REPEATEDLY) ================= */

void loop() {
  updateTime(); //Function not considered in this test, but required for logic operation.
  //In the event of an error, if connected, process particle cloud data to send debug logs.
  if (Particle.connected()) {Particle.process();}; //Process particle data if connected
  //Continue if connected to Particle Cloud
  while (Particle.connected()) {
    delay(2000);
    updateTime(); //Function not considered in this test, but required for logic operation.

    //Print time values on serial
    Serial.println(formattedTime);
    Serial.println(formattedDay);
    Serial.println(period);

    delay(5000);
    updateTime(); //Function not considered in this test, but required for logic operation.
    delay(5000);

    //Checks reservation status and sends appropriate notification.
    //See Logic tables on page 41 for more information.
    if (reservation[day-1][period] == 0) {
      if (pirState == HIGH) {
        //Notification Module not implemented, therefore tested using Serial output.
        Serial.println("Vacant. No movement detected.");
      } else {
        //Notification Module not implemented, therefore tested using Serial output.
        Serial.println("Vacant. Movement detected.");
      }
    } else if (reservation[day-1][period] == 1) {
      //Notification Module not implemented, therefore tested using Serial output.
      Serial.println("Reserved.");
    }
    Particle.process(); //Process Particle Cloud communication data
    break;
  }
}
