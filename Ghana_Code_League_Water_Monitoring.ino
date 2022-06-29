/*
 *This script mainly controls the hardware of the project by 
 *constantly checking for the distance between the ultrasonic sensor 
 *and the target object. Necessary action or actuations are then taken 
 *based on the readings from the sensor
 */

#include "LiquidCrystal.h"
#include <SoftwareSerial.h>

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

SoftwareSerial espSerial(45, 46);

int Echo_pin = 3;
int Trig_pin = 2;

// LCD Parameters
int pingTravelTime;
float pingTravelDistance;
float distanceToTarget;
float dist_litres;
int dt = 2000;

//LED Parameters
int greenLED = 24;
int redLED  =  25;

int buzzer = 6;
unsigned char a;

void setup () {
  Serial.begin(112500);
  Serial.setTimeout(10);

  espSerial.begin(115200);
  
  lcd.begin(16, 2);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);

  pinMode(Echo_pin, INPUT);
  pinMode(Trig_pin, OUTPUT);

  pinMode(buzzer, OUTPUT);
}


float calculate_volume_litres(float val) {
  // 1 litre = 61.02 cubic inch
  // x litres = val cubic inch
  // x litres = (1 * val) / 61.02 cubic inch
  float output_litres = (1 * val) / 61.02;
  return output_litres;
}

void loop() {
  digitalWrite(Trig_pin, LOW);
  delayMicroseconds(10);

  digitalWrite(Trig_pin, HIGH);
  delayMicroseconds(10);

  digitalWrite(Trig_pin, LOW);

  pingTravelTime = pulseIn(Echo_pin, HIGH);
  delay(25);

  pingTravelDistance = (pingTravelTime * 765. * 5280. *12) / (3600. * 1000000);
  distanceToTarget = pingTravelDistance / 2;

  Send_Alert(distanceToTarget);

  Serial.print("Distance to target:  ");
  Serial.print(distanceToTarget);
  Serial.println(" in.");

  // LCD ACTION
  lcd.setCursor(0, 0);
  lcd.print("Water Level:  ");
  lcd.setCursor(0, 1);

  // Converting from inches to litres
  dist_litres = calculate_volume_litres(distanceToTarget);
  lcd.print(dist_litres);
  lcd.print(" LTR");
  delay(dt);
  lcd.clear();
}

void Send_Alert (int distanceToTarget) {
  if (dist_litres <= 0.08) {
    Sound_Buzzer();
//    Serial.write('$');
    espSerial.println('$');
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);
  }

  if (dist_litres >= 0.09) {
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, HIGH);
  }
}

void Sound_Buzzer() {
  for (a = 0; a < 80; a++) {
    digitalWrite(buzzer, HIGH);   delay(1);
    digitalWrite(buzzer, LOW);    delay(1);
  }

  for (a = 0; a < 100; a++) {
    digitalWrite(buzzer, HIGH);   delay(1);
    digitalWrite(buzzer, LOW);    delay(1);
  }
}
