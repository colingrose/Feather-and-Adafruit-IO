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
#define SERVO_PIN 2

// create an instance of the servo class
Servo servo;

// set up the 'servo' feed
AdafruitIO_Feed *servo_feed = io.feed("uv-index.marvista");

// External LED for UV Alert. Hook up your light to pin 14. Without a resister it's very bright, but ok.
#define UValert       14
boolean indicator = false;

// variables for the blinking light
int ledState = LOW; //ledState used to set the LED
// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated
// constants won't change:
long interval = 500;           // interval at which to blink (milliseconds)

int lastValue;

void setup() {

  pinMode(UValert, OUTPUT);
  pinMode(13, OUTPUT); //to include a reset button, hook one lead to RST, one lead to pin 13 (ground)

  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while (! Serial);

  // tell the servo class which pin we are using
  servo.attach(SERVO_PIN);
  servo.write(0);
  delay(1000);
  servo.write(180);
  delay(1000);
  servo.write(0);


  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // set up a message handler for the 'servo' feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  servo_feed->onMessage(handleMessage);

  // wait for a connection
  while (io.status() < AIO_CONNECTED) {
    //Serial.print(".");
    Serial.println(io.statusText());  //troubleshooting the endless dots when connecting...........
    digitalWrite(UValert, HIGH);
    delay(50);
    digitalWrite(UValert, LOW);
    delay(50);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  servo_feed->get();

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
// this function is called whenever a 'servo' message
// is received from Adafruit IO. it was attached to
// the servo feed in the setup() function above.
void handleMessage(AdafruitIO_Data *data) {

  // convert the data to integer
  int uvIndex = data->toInt();

  //print the UV Index
  Serial.print("UV index is ");
  Serial.println(uvIndex);

  // make sure we don't exceed the limit
  // of the servo. the range is from 0
  // to 11.
  if (uvIndex < 0)
    uvIndex = 0;
  else if (uvIndex > 11)
    uvIndex = 11;
  //turn on the UV alert light if the UV value is 6 or higher
  if (uvIndex >= 6) {
    indicator = true;
  }
  else {
    indicator = false;
  }
  if (lastValue != uvIndex) {
    //map the uvIndex from 0-11 to an angle between 180-0
    int uvIndexPosition = map(uvIndex, 0, 11, 170, 0);

    //set the servo to the correct angle
    servo.write(uvIndexPosition);
    lastValue = uvIndex;
  }
}

void blink() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(UValert, ledState);
  }
}
