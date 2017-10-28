#include <stdio.h>
#include <stdlib.h>
#include <Servo.h>

//define pins here
#define TRIG_LEFT 2
#define ECHO_LEFT 3

#define TRIG_RIGHT 4
#define ECHO_RIGHT 5

#define TRIG_FRONT 6
#define ECHO_FRONT 7

#define TURN_PIN 9
#define DRIVE_PIN 11

#define MIN_DIST          15 // cm
#define MAX_DIST          20 // cm
#define MAX_RANGE         50 // cm
#define FRONT_DIST        25 // cm
#define LEFT_SIDE         1
#define RIGHT_SIDE        2

int left_dist, right_dist, front_dist, wall_side;
char str[100];
int timeout = 11600; // microseconds -> equivalent to 4m
Servo servo;

void setup() {
  Serial.begin(9600);

  servo.attach(TURN_PIN);
  pinMode(DRIVE_PIN, OUTPUT);

  usonicsetup();
  delay(200);  // for sensors to settle

  // Determine the wall to follow
  left_dist = usonic(timeout, TRIG_LEFT) / 58;
  right_dist = usonic(timeout, TRIG_RIGHT) / 58;
  front_dist = usonic(timeout, TRIG_FRONT) / 58;

  if (left_dist < MAX_RANGE || right_dist < MAX_RANGE) {
    if (left_dist < right_dist) {
      wall_side = LEFT_SIDE;
    } else {
      wall_side = RIGHT_SIDE;
    }
  } else {
    wall_side = -1;
  }
  analogWrite(DRIVE_PIN, 5);
  
}

void loop() {
  left_dist = usonic(timeout, TRIG_LEFT) / 58;
  right_dist = usonic(timeout, TRIG_RIGHT) / 58;
  front_dist = usonic(timeout, TRIG_FRONT) / 58;

  /*if (left_dist < MIN_DIST && right_dist < MIN_DIST && front_dist < FRONT_DIST) {
    Serial.print("All sensors blocked - shutting down!\n");
    straight();
    analogWrite(DRIVE_PIN, 0);
    exit(0);
  }*/

  if (wall_side == -1) {
    if (front_dist > FRONT_DIST) {
      straight();
    } else {
      turnLeft();
      wall_side = RIGHT_SIDE;
    }
  } else if (wall_side == LEFT_SIDE) {
    if (left_dist < MIN_DIST || front_dist < FRONT_DIST)    { turnRight(); }
    else if (left_dist > MAX_DIST)                          { turnLeft(); }
    else                                                    { straight(); }
  } else if (wall_side == RIGHT_SIDE) {
    if (right_dist < MIN_DIST || front_dist < FRONT_DIST)   { turnLeft(); }
    else if (right_dist > MAX_DIST)                         { turnRight(); }
    else                                                    { straight(); }
  }
  
  sprintf(str, "wall_side = %s left = %d, right = %d, front = %d\n", wall_side == 9 ? "LEFT" : "RIGHT", left_dist, right_dist, front_dist);
  Serial.print(str);
  delay(100);
}

/*
 *  Drive Functions
 */
void turnRight(){
  servo.write(180);
}

void turnLeft(){
  servo.write(0);
}

void straight(){
  servo.write(90);
}


/*
 *  ultra sonic pin setup
 */
void usonicsetup(void) {
 pinMode(ECHO_LEFT, INPUT);
 pinMode(TRIG_LEFT, OUTPUT);

 pinMode(ECHO_RIGHT, INPUT);
 pinMode(TRIG_RIGHT, OUTPUT);

 pinMode(ECHO_FRONT, INPUT);
 pinMode(TRIG_FRONT, OUTPUT);
 
 digitalWrite(TRIG_LEFT, LOW);
 digitalWrite(TRIG_RIGHT, LOW);
}


/*  utimeout is maximum time to wait for return in us
 * 
 */
long usonic(long utimeout, int trig_pin) {
 
 long b;
 int echo = trig_pin + 1;
 
 //if echo line is still low from last result, return 0;
 if(digitalRead(echo) == HIGH) return 0;

 //send trigger pulse
 digitalWrite(trig_pin, HIGH); 
 delay(1);
 digitalWrite(trig_pin, LOW);
 
 long utimer = micros();
 
 //wait for pin state to change - return starts after 460us typically or timeout (eg if not connected)
 while((digitalRead(echo) == LOW) && ((micros() - utimer) < 1000)){}
  
 utimer = micros();
 
 //wait for pin state to change
 while((digitalRead(echo) == HIGH) && ((micros() - utimer) < utimeout)){}
 
 b = micros() - utimer;
 if(b == 0) b = utimeout;
 return b;
 
}
