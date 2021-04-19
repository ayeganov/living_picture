//#define TIMER_ENABLED true
//#include <NewPing.h>
////#include <Servo.h>
//#include "Arduino.h"
//#include "filters.hpp"
//#include "PCM.h"
//
//
//const int ECHO_PIN = 7;
//const int TRIGGER_PIN = 6;
//const int MAX_DISTANCE_CM = 300;
//
//NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE_CM);
//filter::Complementary sonic_filter(0.5);
//
////Servo myservo;  // create servo object to control a servo
//
//int potpin = 0;  // analog pin used to connect the potentiometer
//int val;    // variable to read the value from the analog pin
//unsigned long ping_timer = 0;
//unsigned long ping_speed = 30;
//const long BAUD_RATE = 115200;
//const float SOUND_THRESHOLD = 542;
//const int SOUND_SENSOR_PIN = A1;
//const int SOUND_SENSOR_DIG_PIN = 2;
//float current_distance = 0;
//bool do_sense_sound = false;
//unsigned long last_event = 0;
//
//
//void sense_sound()
//{
//  float sensor_value = analogRead(SOUND_SENSOR_PIN);
//  float abs_value = abs(sensor_value - SOUND_THRESHOLD);
//
//  int sensor_data = digitalRead(SOUND_SENSOR_DIG_PIN);
//  if(sensor_data == LOW)
//  {
//    auto passed_time = millis() - last_event;
//    if(passed_time > 25)
//    {
//      Serial.print("Detected sound: ");
//      Serial.println(abs_value, 2);
//      last_event = millis();
//    }
//  }
//}
//
//
//void echo_check()
//{
//  if(sonar.check_timer())
//  {
//    float current_distance = sonic_filter.filter(sonar.ping_result / US_ROUNDTRIP_CM);
////    Serial.print("Distance: ");
////    Serial.print(current_distance);
////    Serial.println("cm");
////
//    if(current_distance < 30)
//    {
//      do_sense_sound = true;
//    }
//    else
//    {
//      do_sense_sound = false;
//      Serial.println("Ignoring sound");
//    }
//  }
//}
//
//
//void setup()
//{
//  Serial.begin(BAUD_RATE);
////  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
//  ping_timer = last_event = millis();
//}
//
//
//
//void loop()
//{
//  if(millis() >= ping_timer)
//  {
//    ping_timer += ping_speed;
//    sonar.ping_timer(echo_check);
//  }
//
//  if(do_sense_sound)
//  {
//    sense_sound();
//  }
//
////  val = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
////  val = map(val, 0, 2048, 0, 1024);     // scale it to use it with the servo (value between 0 and 180)
////  myservo.write(val);                  // sets the servo position according to the scaled value
////  delay(10);                           // waits for the servo to get there
//}


//#include "Arduino.h"
//#include <AccelStepper.h>
//
//
//const long BAUD_RATE = 115200;
//const int FULL_STEP = 4;
//const unsigned int IN1 = 10, IN2 = 11, IN3 = 12, IN4 = 13;
//
//AccelStepper stepper = AccelStepper(AccelStepper::FULL4WIRE, IN1, IN2, IN3, IN4);
//long last_pos = 1000;
//
//
//void setup()
//{
//  Serial.begin(BAUD_RATE);
//  stepper.setMaxSpeed(2000);
//  stepper.setAcceleration(50);
//  stepper.setSpeed(500);
//  stepper.moveTo(2000);
//  Serial.print("Is running: ");
//  Serial.println(stepper.isRunning());
//}
//
//
//void loop()
//{
//  if(stepper.distanceToGo() == 0)
//  {
//    Serial.print("Reversing the direction: ");
//    Serial.println(stepper.currentPosition());
//    stepper.moveTo(-stepper.currentPosition());
//  }
//
//  stepper.run();
//}

#include "Arduino.h"

#define STEPPER_PIN_1 10
#define STEPPER_PIN_2 11
#define STEPPER_PIN_3 12
#define STEPPER_PIN_4 13

void OneStep(bool dir);

const int NUM_STEPS = 2000;
int steps_left = NUM_STEPS;
bool go_right = true;
int step_number = 0;


void setup()
{
  Serial.begin(115200);
  pinMode(STEPPER_PIN_1, OUTPUT);
  pinMode(STEPPER_PIN_2, OUTPUT);
  pinMode(STEPPER_PIN_3, OUTPUT);
  pinMode(STEPPER_PIN_4, OUTPUT);
}


void loop()
{
  bool reverse = steps_left == 0;
  if(reverse)
  {
    Serial.println("Reversing direction");
    go_right = !go_right;
    steps_left = NUM_STEPS;
  }

  OneStep(go_right);
  --steps_left;
  delay(2);
}


void OneStep(bool dir)
{
  if(dir)
  {
    switch(step_number)
    {
      case 0:
        digitalWrite(STEPPER_PIN_1, HIGH);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 1:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, HIGH);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 2:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, HIGH);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 3:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, HIGH);
        break;
    }
  }
  else
  {
    switch(step_number)
    {
      case 0:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, HIGH);
        break;
      case 1:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, HIGH);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 2:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, HIGH);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 3:
        digitalWrite(STEPPER_PIN_1, HIGH);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
    }
  }

  ++step_number;
  if(step_number > 3)
  {
    step_number = 0;
  }
}
