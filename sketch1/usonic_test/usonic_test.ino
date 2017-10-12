// Ultrasonic HC-SR04 unit interface
// Uses serial port at 115200 baud for communication
// use trig pin for output, echo pin for input
// pulse out (10us) on trig initiates a cycle
// pulse width returned on echo is proportional to distance
// specs say 38ms = no return (beyond limit), but 90ms and more have been seen
// set utimeout when calling usonic (routine will take longer for longer returns)
// higher timeout measures further, but can take longer if no echo
// if return >= utimeout, no valid pulse received
// if return < ~100 unit is faulty/disconnected (routine is timing out waiting for start of return)
// if return == 0 then unit is still sending return from last ping (or is faulty)
// maximum nominal range is 5m => utimeout = 29000 us
// call usonicsetup() during setup
// call usonic(timeout) to get return time in microseconds
// divide result of usonic by 58 to get range in cm

//define pins here
#define TRIG_LEFT 2
#define ECHO_LEFT 3

#define TRIG_RIGHT 4
#define ECHO_RIGHT 5

#define TRIG_FRONT 6
#define ECHO_FRONT 7

#define USMAX 3000

#define LEFT_TURN_PIN 9
#define RIGHT_TURN_PIN 10
#define DRIVE_PIN 11


#define MIN_DIST    150 // mm
#define MAX_DIST    160 // mm

#define CORRECT_ANGLE   5 // degrees

#define STEP_DIST   5   // mm
#define STEP_DIST_SPEED   250 // mm
#define STEP_TURN   -90   // degrees
#define STEP_TURN_SPEED   45    // degrees

#define FRONT_PSD   2 // Front distance sensor
#define LEFT_PSD    1 // Left distance sensor

void setup() {
  
  Serial.begin(9600);
 
 // initialize digital pin LED_BUILTIN as an output.
 pinMode(LED_BUILTIN, OUTPUT);
 
 pinMode(LEFT_TURN_PIN, OUTPUT);
 pinMode(RIGHT_TURN_PIN, OUTPUT);
 pinMode(DRIVE_PIN, OUTPUT);
 
 usonicsetup();            //set up ultrasonic sensor

 analogWrite(DRIVE_PIN, 5); //drive at slower speed
 
}

void loop() {
  
 int d_left, d_right;

 //distance in cm, time out at 11600us or 2m maximum range
 d_left = usonic(11600, 2) / 58;
 d_right = usonic(11600, 4) / 58;

 Serial.print(d_left);
 Serial.print("\n");
 
 Serial.print(d_right);
 Serial.print("\n");
 
 if (d_left < 10) {
  digitalWrite(RIGHT_TURN_PIN, HIGH);
 } 
 if (d_right < 10) {
  digitalWrite(LEFT_TURN_PIN, HIGH);
 }
 delay(100);              
 //TODO findout if we actually need this when not using serial port

  int forward_dist = PSDGet(FRONT_PSD);
  int left_dist = PSDGet(LEFT_PSD);

  while (current_turn < NUM_TURNS) {
    VWStraight(STEP_DIST, STEP_DIST_SPEED);
    if (forward_dist < MIN_DIST) {
      VWTurn(STEP_TURN, STEP_TURN_SPEED);
      VWWait();
      current_turn++;
    } else if (left_dist < MIN_DIST) {
      VWTurn(-1*CORRECT_ANGLE, STEP_TURN_SPEED);
    } else if (left_dist > MAX_DIST) {
      VWTurn(CORRECT_ANGLE, STEP_TURN_SPEED);
    }
    forward_dist = PSDGet(FRONT_PSD);
    left_dist = PSDGet(LEFT_PSD);
  }
}

void usonicsetup(void) {
 pinMode(ECHO_LEFT, INPUT);
 pinMode(TRIG_LEFT, OUTPUT);

 pinMode(ECHO_RIGHT, INPUT);
 pinMode(TRIG_RIGHT, OUTPUT);
 
 digitalWrite(TRIG_LEFT, LOW);
 digitalWrite(TRIG_RIGHT, LOW);
}

//utimeout is maximum time to wait for return in us
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
