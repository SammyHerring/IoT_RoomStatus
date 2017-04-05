//Any earlier values are required for optimal operation, but may be discounted from this test.

/* ================ SETUP CODE (RUNS ONCE) ==================== */

void setup() {

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
