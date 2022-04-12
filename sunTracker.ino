/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"

/************************ Code Starts Here *******************************/

#if defined(ARDUINO_ARCH_ESP32)
// ESP32Servo Library (https://github.com/madhephaestus/ESP32Servo)
// installation: library manager -> search -> "ESP32Servo"
#include <ESP32Servo.h>
#else
#include <Servo.h>
#endif

// pin used to control the servo
#define SERVOAZ_PIN 2
#define SERVOAL_PIN 16

// create an instance of the servo class
Servo servoAZ;
Servo servoAL;

// set up the 'servo' feed
AdafruitIO_Feed *servo_feedAZ = io.feed("sun-position.azimuth");
AdafruitIO_Feed *servo_feedAL = io.feed("sun-position.altitude");

// External LED for UV Alert. Hook up your light to pin 14. Without a resister it's very bright, but ok.
#define alert       13
boolean indicator = false;

// variables for the blinking light
int ledState = LOW; //ledState used to set the LED
// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated
// constants won't change:
long interval = 500;           // interval at which to blink (milliseconds)

int lastAZValue;
int lastALValue;

void setup() {

  pinMode(alert, OUTPUT);
  pinMode(13, OUTPUT); //to include a reset button, hook one lead to RST, one lead to pin 13 (ground)

  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while (! Serial);

  // tell the servo class which pin we are using
  servoAZ.attach(SERVOAZ_PIN);
  servoAL.attach(SERVOAL_PIN);

  //test the servos
  servoAZ.write(0);
  delay(1000);
  servoAZ.write(180);
  delay(1000);
  servoAZ.write(0);
  delay(1000);
  servoAL.write(0);
  delay(1000);
  servoAL.write(180);
  delay(1000);
  servoAL.write(0);


  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // set up a message handler for the 'servo' feeds.
  // the handleMessage functions (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  servo_feedAZ->onMessage(handleMessageAZ);
  servo_feedAL->onMessage(handleMessageAL);

  // wait for a connection
  while (io.status() < AIO_CONNECTED) {
    //Serial.print(".");
    Serial.println(io.statusText());  //troubleshooting the endless dots when connecting...........
    digitalWrite(alert, HIGH);
    delay(50);
    digitalWrite(alert, LOW);
    delay(50);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  servo_feedAZ->get();
  servo_feedAL->get();

}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();
}


// this function is called whenever an 'azimuth' message
// is received from Adafruit IO. it was attached to
// the servo feed in the setup() function above.
void handleMessageAZ(AdafruitIO_Data *data) {

  // convert the data to integer
  int Azimuth = data->toInt();

  // make sure the azimuth is only used between east (90) and west (270) during the day
  if (Azimuth < 90)
    Azimuth = 90;
  else if (Azimuth > 270)
    Azimuth = 270;

  if (lastAZValue != Azimuth) {
    //map the azimuth from east to west (daytime) to 0-180 on the servo
    int AZServoPosition = map(Azimuth, 90, 270, 180, 0);

    //set the servo to the correct angle
    servoAZ.write(AZServoPosition);
    lastAZValue = Azimuth;

    //print the Azimuth
    Serial.print("Azimuth is ");
    Serial.print(Azimuth);
    Serial.print(" (servo position ");
    Serial.print(AZServoPosition);
    Serial.print(")");
  }
}


// this function is called whenever an 'altitude' message
// is received from Adafruit IO. it was attached to
// the servo feed in the setup() function above.
void handleMessageAL(AdafruitIO_Data *data) {

  // convert the data to integer
  int Altitude = data->toInt();

  //print the Altitude
  Serial.print(" and Altitude is ");
  Serial.println(Altitude);

  if (lastALValue != Altitude) {

    //set the servo to the correct angle
    servoAL.write(Altitude);
    lastALValue = Altitude;
  }
}
