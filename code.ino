#include <Servo.h>
#include <SoftwareSerial.h>

const int motionPin = 2;      // hc-sr501 pir sensor pin
const int buzzerPin = 8;      // buzzer pin
const int servoPin = 9;       // servo motor pin

Servo servo;
SoftwareSerial gsm(7, 6);     // sim800l RX, TX pins

int motionStatus = 0;
unsigned long lastMotionTime = 0;
const unsigned long timeLimit = 300000; // 5 minutes threshold for suffocation detection

void setup() {
  pinMode(motionPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  servo.attach(servoPin);
  gsm.begin(9600);
  
  servo.write(0);  // set servo to initial position (window/vent closed)
}

void loop() {
  motionStatus = digitalRead(motionPin);
  
  if (motionStatus == HIGH) {
    lastMotionTime = millis();  // update the time when motion was detected
  }
  
  if (millis() - lastMotionTime > timeLimit) {
    // suffocation condition detected: No motion for more than the time limit
    triggerAlarm();
    sendSMS();
    activateVentilation();
  }
}

void triggerAlarm() {
  digitalWrite(buzzerPin, HIGH);  // sound the alarm
  delay(1000);  // buzzer sound for 1 sec
  digitalWrite(buzzerPin, LOW);   //stop the buzzer
}

void sendSMS() {
  gsm.println("AT+CMGF=1");   // set sms mode
  delay(100);
  gsm.println("AT+CMGS=\"+1234567890\"");  // replace with the emergency number
  delay(100);
  gsm.println("Suffocation warning! Please check the vehicle immediately.");
  delay(100);
  gsm.write(26);  // send the sms
}

void activateVentilation() {
  servo.write(90);  // rotate servo to open window/vent (adjust based on your mechanism)
  delay(1000);
  servo.write(0);   // close the window/vent after 1 sec
}
