/*
 * Program: adf_sensor_actuator.ino
 * Developer: Aditya Prakash
 * Board: Arduino Nano
*/
#include "HX711.h"
#include "Servo.h"

HX711 dog_scale, bowl_scale;
//Variables for Dog weight scale
int scale1_dt_pin = 2;
int scale1_sck_pin = 3;
int dog_wgt_thresh = 300000;//3kg
int scale1_calib_fac = -573; //(assumed)

//Variables for Bowl weight scale
int scale2_dt_pin = 4;
int scale2_sck_pin = 5;
int bowl_wgt_thresh = 500;//500g (assumed)
int scale2_calib_fac = -537; //(assumed)

//Ultrasonic variables
int echo_pin = 6;
int trig_pin = 7;
long duration,distance;
long dist_thresh = 200; //in cms(assumed)

//Servo variables
int food_servo_pin = 1;
int water_servo_pin = 2;
Servo food_servo, water_servo;
int max_angle = 120; //(assumed)
int min_angle = 0;  //(assumed)

//Flags
int dog_flag = 0;
int bowl_flag = 0;
int ultra_flag = 0;


void setup() {
  //Setup serial
  Serial.begin(9600);
  
  //Setup weight sensors - 2
  dog_scale.begin(scale1_dt_pin,scale1_sck_pin);
  bowl_scale.begin(scale2_dt_pin,scale2_sck_pin);
  
  dog_scale.set_scale(scale1_calib_fac);
  bowl_scale.set_scale(scale2_calib_fac);
  
  //Setup ultrasonic
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  
  //Setup servos
  pinMode(food_servo_pin, OUTPUT);
  pinMode(water_servo_pin, OUTPUT);
  food_servo.attach(food_servo_pin);
  water_servo.attach(water_servo_pin);
  food_servo.write(min_angle);
  water_servo.write(min_angle);
}

void loop() {
  //Scan the 2 weight sensors
  dog_scale.tare();
  bowl_scale.tare();
  if(dog_scale.get_units(10) > dog_wgt_thresh){dog_flag = 1;}
  else {dog_flag = 0;}

  if(bowl_scale.get_units(10) > bowl_wgt_thresh){bowl_flag = 1;}
  else {bowl_flag = 0;}
  
  //If dog weight detected, get ultrasound value and 
  //if dog weight sensor and ultrasound sensor are triggered,
  //send serial message to RPi of format "_ _ \n", where _ can
  //be 0 or 1
  if (dog_flag == 1)
  {  
    for(int i=0; i<5; i++)
    {
      digitalWrite(trig_pin, LOW);
      delayMicroseconds(2);
      digitalWrite(trig_pin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trig_pin, LOW);
      duration = pulseIn(echo_pin, HIGH);
      distance += duration * 0.034 / 2;
    }
    int dist_avg = distance/5;
    if(dist_avg < dist_thresh)
    {ultra_flag = 1;}
    else
    {ultra_flag = 0;}
  }

  if (dog_flag == 1 && ultra_flag == 1 && bowl_flag == 0)
  {Serial.print("10\n");}
  else if (dog_flag == 1 && ultra_flag == 1 && bowl_flag == 1)
  {Serial.print("11\n");}
  else
  {Serial.print("00\n");}
   
  //Wait for message from RPi to either move the servos or
  //bring them to home
  if(Serial.read() == '1')
  {
    food_servo.write(max_angle);
    water_servo.write(max_angle);
  }
  else if(Serial.read() == '0')
  {
    food_servo.write(min_angle);
    water_servo.write(min_angle);
  }
  
  
}
