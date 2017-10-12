#include <stdio.h>
#include <stdlib.h>

//define pins here
#define TRIG_LEFT 2
#define ECHO_LEFT 3

#define TRIG_RIGHT 4
#define ECHO_RIGHT 5

#define TRIG_FRONT 6
#define ECHO_FRONT 7

#define LEFT_TURN_PIN 9
#define RIGHT_TURN_PIN 10
#define DRIVE_PIN 11

#define MIN_DIST    15 // cm
#define MAX_DIST    16 // cm

bool wallFound = false;
int d_left, d_right, d_front; // cm

void setup() {
  
 Serial.begin(9600);
 
 // initialize digital pin LED_BUILTIN as an output.
 pinMode(LED_BUILTIN, OUTPUT);
 
 pinMode(LEFT_TURN_PIN, OUTPUT);
 pinMode(RIGHT_TURN_PIN, OUTPUT);
 pinMode(DRIVE_PIN, OUTPUT);
 
 usonicsetup();            //set up ultrasonic sensor
 
}

/*
 *  Turn Functions
 */
void turnRight(){
  digitalWrite(LEFT_TURN_PIN, LOW);
  digitalWrite(RIGHT_TURN_PIN, HIGH);
}

void turnLeft(){
  digitalWrite(RIGHT_TURN_PIN, LOW);
  digitalWrite(LEFT_TURN_PIN, HIGH);
}

void straight(){
  digitalWrite(RIGHT_TURN_PIN, LOW);
  digitalWrite(LEFT_TURN_PIN, LOW);
}

/*
 *  Main Loop
 */
void loop() {

  //distance in cm, time out at 11600us or 2m maximum range
  d_left = usonic(11600, 2) / 58;
  d_right = usonic(11600, 4) / 58;
  d_front = usonic(11600, 4) / 58;

  if(d_left < 10 && d_right < 10 && d_front < 10) exit(0);

  char str[100];
  sprintf(str, "d_left = %d, d_right = %d, d_front = %d\n", d_left, d_right, d_front);
  Serial.print(str);

  if (wallFound == false){

    analogWrite(DRIVE_PIN, 5);
    if (d_right < MIN_DIST) {
      turnLeft();
    } else {
      turnRight();
    }

    if(d_front > 200){
      wallFound = true;
    }
    
  } else if (d_left < 200) {
    
    analogWrite(DRIVE_PIN, 5);
    if (d_left < MIN_DIST) {
      turnRight();
    } else if (d_left > MAX_DIST) {
      turnLeft();
    } else {
      straight();
    }
    
  } else {

    d_front = usonic(11600, 4) / 58;
    turnLeft();
    
  }
  
}

/*
 *  ultra sonic pin setup
 */
void usonicsetup(void) {
 pinMode(ECHO_LEFT, INPUT);
 pinMode(TRIG_LEFT, OUTPUT);

 pinMode(ECHO_RIGHT, INPUT);
 pinMode(TRIG_RIGHT, OUTPUT);
 
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
