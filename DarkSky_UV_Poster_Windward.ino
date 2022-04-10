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
#define SERVOAZIMUTH_PIN 2
#define SERVOALTITUDE_PIN 3

// create two instances of the servo class
Servo servoAzimuth;
Servo servoAltitude;

// set up the 'servo' feeds
AdafruitIO_Feed *servoAzimuth_feed = io.feed("sun-position.azimuth");
AdafruitIO_Feed *servoAltitude_feed = io.feed("sun-position.altitude");

void setup() {

  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while (! Serial);

  // tell the servo class which pin we are using
  servoAzimuth.attach(SERVOAZIMUTH_PIN);
  servoAltitude.attach(SERVOALTITUDE_PIN);
  servoAzimuth.write(0);
  delay(1000);
  servoAzimuth.write(359);
  delay(1000);
  servoAzimuth.write(0);
  delay(1000);
  servoAltitude.write(0);
  delay(1000);
  servoAltitude.write(90);
  delay(1000);
  servoAltitude.write(0);
  delay(1000);


  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // set up a message handler for the 'servo' feeds.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  servoAzimuth_feed->onMessage(handleMessage);
  servoAltitude_feed->onMessage(handleMessage);

  // wait for a connection
  while (io.status() < AIO_CONNECTED) {
    //Serial.print(".");
    Serial.println(io.statusText());  //troubleshooting the endless dots when connecting...........
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  servoAzimuth_feed->get();
  servoAltitude_feed->get();

}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();
  if (indicator == true) {
    blink();
  }
  else {
    digitalWrite(UValert, LOW);
  }
}
// this function is called whenever an 'Azimuth' message
// is received from Adafruit IO. it was attached to
// the azimuth servo feed in the setup() function above.
void handleMessage(AdafruitIO_Data *servoAzimuth_feed) {

  // convert the data to integer
  int azimuth = data->toInt();

  //print the Azimuth
  Serial.print("Azimuth is ");
  Serial.println(azimuth);

  //set the servo to the correct angle
  servoAzimuth.write(azimuth);
  }
}
// this function is called whenever an 'Altitude' message
// is received from Adafruit IO. it was attached to
// the altitude servo feed in the setup() function above.
void handleMessage(AdafruitIO_Data *servoAltitude_feed) {

  // convert the data to integer
  int altitude = data->toInt();

  //print the Azimuth
  Serial.print("Altitude is ");
  Serial.println(altitude);

  //set the servo to the correct angle
  servoAltitude.write(altitude);
  }
}
