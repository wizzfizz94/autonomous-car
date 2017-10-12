void setup() {
  // put your setup code here, to run once:

}

void loop() {
  
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  float voltage = sensorValue * (5.0 / 1023.0);
  // print out the value you read:
  Serial.println(voltage);
  
  int r_mot = calcPID();
  setMotorOutput()

  if(//){
    //turn right
  else if(//){
    //turn left
  } else {
    //go straight
  }
}

int getSensorValue(){
  return 0;
}

int calcPID(){
  return 0;  
}

int setMotorOutput(){
  return 0;
}
