// when installing the library for the ESP8266 boards you may run into problems. I got this code working
// using version 2.7.4 of the library (using a PC)


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
AdafruitIO_Feed *feedAZ = io.feed("sun-position.azimuth");
AdafruitIO_Feed *feedAL = io.feed("sun-position.altitude");

void setup() {
  // start the serial connection
  Serial.begin(115200);
  // wait for serial monitor to open
  while (! Serial);
  Serial.println("Hello!");
  
  // tell the azimuth servo class which pin we are using and test
  servoAZ.attach(SERVOAZ_PIN);
  //test the servos
  Serial.println("Testing azimuth servo");
  servoAZ.write(0);
  delay(1000);
  servoAZ.write(180);
  delay(1000);
  servoAZ.write(0);
  delay(1000);
  // tell the altitude servo class which pin we are using and test
  servoAL.attach(SERVOAL_PIN);  //don't send this servo close to 0 or it will get stuck!
  Serial.println("Testing altitude servo");
  servoAL.write(90);
  delay(1000);
  servoAL.write(180);
  delay(1000);
  servoAL.write(90);
  delay(1000);

  // connect to io.adafruit.com
  Serial.println("Connecting to Adafruit IO");
  io.connect();

  // set up a message handler for the 'servo' feeds.
  // the handleMessage functions (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  feedAZ->onMessage(handleMessageAZ);
  feedAL->onMessage(handleMessageAL);

  // wait for a connection
  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    //Serial.println(io.statusText());  //troubleshooting the endless dots when connecting...........
    delay(100);
  }
  // we are connected
  Serial.println(io.statusText());
  //get data from feeds
  feedAZ->get();
  feedAL->get();
}

void loop() {
  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();
}

/**************************get azimuth data ***********************************/
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
  //map the azimuth from east to west (daytime) to 0-180 on the servo
  int AZServoPosition = map(Azimuth, 90, 270, 180, 0);
  //set the servo to the correct angle
  servoAZ.write(AZServoPosition);
  delay(1000);
  //print the Azimuth
  Serial.print("Azimuth is ");
  Serial.print(Azimuth);
  Serial.print(" (servo position ");
  Serial.print(AZServoPosition);
  Serial.println(")");
}

/**************************get altitude data ***********************************/
// this function is called whenever an 'altitude' message
// is received from Adafruit IO. it was attached to
// the servo feed in the setup() function above.
void handleMessageAL(AdafruitIO_Data *data2) {
  // convert the data to integer
  int Altitude = data2->toInt();
  //print the Altitude
  Serial.print("Altitude is ");
  Serial.print(Altitude);
  //Map the Altitude from 0-90 to values that work with the altitude servo. Lots of trail and error.
  //I found that giving a servo value of 105 would put the pointer straight up (elevation 90)
  //and a servo value of 140 would put the pointer at elevation 56, which was convenient at the time of testing
  int ALServoPosition = map(Altitude, 56, 90, 140, 105);
  //set the servo to the correct angle
  servoAL.write(ALServoPosition);
  delay(1000);
  //print the servo position
  Serial.print(" (servo position ");
  Serial.print(ALServoPosition);
  Serial.println(")");
}
