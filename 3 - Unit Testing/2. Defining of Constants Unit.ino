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

//The setup and loop components added below to allow the file to successfully compile.
//However, this unit does not run within them as they are apart of the system setup.

/* ================ SETUP CODE (RUNS ONCE) ==================== */

void setup() {

}

/* ================ MAIN CODE (RUN REPEATEDLY) ================= */

void loop() {

}
