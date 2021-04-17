#define TIMER_ENABLED true
#include <NewPing.h>
#include <Servo.h>
#include "Arduino.h"
#include "filters.hpp"


const int ECHO_PIN = 7;
const int TRIGGER_PIN = 6;
const int MAX_DISTANCE_CM = 300;

Servo myservo;  // create servo object to control a servo
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE_CM);
filter::Complementary sonic_filter(0.5);

int potpin = 0;  // analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin
unsigned long ping_timer = 0;
unsigned long ping_speed = 30;
const long BAUD_RATE = 115200;


void echo_check()
{
  if(sonar.check_timer())
  {
    float filtered_value = sonic_filter.filter(sonar.ping_result / US_ROUNDTRIP_CM);
    Serial.print("Pint: ");
    Serial.print(filtered_value);
    Serial.println("cm");

    if(filtered_value < 30)
    {
      myservo.write(280);
    }
    else
    {
      myservo.write(0);
    }
  }
}


void setup() {
  Serial.begin(BAUD_RATE);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  ping_timer = millis();
}



void loop()
{
  if(millis() >= ping_timer)
  {
    ping_timer += ping_speed;
    sonar.ping_timer(echo_check);
  }
//  val = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
//  val = map(val, 0, 2048, 0, 1024);     // scale it to use it with the servo (value between 0 and 180)
//  myservo.write(val);                  // sets the servo position according to the scaled value
//  delay(10);                           // waits for the servo to get there
}


