#include <SoftwareSerial.h>

// Define pins for Ultrasonic Sensors (Side 1, 3, 4)
#define TRIG1 2
#define ECHO1 5
#define TRIG2 3
#define ECHO2 6
#define TRIG3 4
#define ECHO3 7

// Define pins for PIR Sensors
#define PIR1 8  // Side 1
#define PIR2 9  // Side 2
#define PIR3 10 // Side 3
#define PIR4 11 // Side 4

// Define pins for Buzzer and Flashlights
#define BUZZER 12
#define LIGHT1 A0
#define LIGHT2 A1
#define LIGHT3 A2
#define LIGHT4 A3

// Define pin for Magnetic Reed Sensor (Gate security)
#define REED_SENSOR 13

// GSM Module
SoftwareSerial gsm(A4, A5); // RX, TX for GSM module

// Phone number to send SMS (Change this to the owner's phone number)
const String ownerPhoneNumber = "+916361240104"; // Replace with actual phone number

void setup() {
  Serial.begin(9600);  // Start Serial Monitor communication
  gsm.begin(9600);

  // Configure Ultrasonic Sensors
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);
  pinMode(TRIG3, OUTPUT);
  pinMode(ECHO3, INPUT);

  // Configure PIR Sensors
  pinMode(PIR1, INPUT);
  pinMode(PIR2, INPUT);
  pinMode(PIR3, INPUT);
  pinMode(PIR4, INPUT);

  // Configure Outputs
  pinMode(BUZZER, OUTPUT);
  pinMode(LIGHT1, OUTPUT);
  pinMode(LIGHT2, OUTPUT);
  pinMode(LIGHT3, OUTPUT);
  pinMode(LIGHT4, OUTPUT);

  // Configure Magnetic Reed Sensor
  pinMode(REED_SENSOR, INPUT_PULLUP);

  // Initialize Outputs
  digitalWrite(BUZZER, LOW);
  digitalWrite(LIGHT1, LOW);
  digitalWrite(LIGHT2, LOW);
  digitalWrite(LIGHT3, LOW);
  digitalWrite(LIGHT4, LOW);

  // GSM Module Initialization Message
  sendSMS("Smart Agriculture Protection System is Activated.");
}

void loop() {
  bool intrusionDetected = false;

  // Check Ultrasonic Sensors for Object Detection
  int distance1 = checkUltrasonic(TRIG1, ECHO1);
  int distance2 = checkUltrasonic(TRIG2, ECHO2);
  int distance3 = checkUltrasonic(TRIG3, ECHO3);

  // Display ultrasonic sensor readings on Serial Monitor
  Serial.print("Side 1 Ultrasonic Distance: ");
  Serial.print(distance1);
  Serial.println(" cm");

  Serial.print("Side 3 Ultrasonic Distance: ");
  Serial.print(distance2);
  Serial.println(" cm");

  Serial.print("Side 4 Ultrasonic Distance: ");
  Serial.print(distance3);
  Serial.println(" cm");

  // Display PIR sensor status on Serial Monitor
  Serial.print("PIR Sensor 1 (Side 1): ");
  Serial.println(digitalRead(PIR1) == HIGH ? "Motion Detected" : "No Motion");

  Serial.print("PIR Sensor 2 (Side 2): ");
  Serial.println(digitalRead(PIR2) == HIGH ? "Motion Detected" : "No Motion");

  Serial.print("PIR Sensor 3 (Side 3): ");
  Serial.println(digitalRead(PIR3) == HIGH ? "Motion Detected" : "No Motion");

  Serial.print("PIR Sensor 4 (Side 4): ");
  Serial.println(digitalRead(PIR4) == HIGH ? "Motion Detected" : "No Motion");

  // Check Magnetic Reed Sensor for Circuit Break
  bool reedSensorStatus = (digitalRead(REED_SENSOR) == LOW);
  Serial.print("Magnetic Reed Sensor Status: ");
  Serial.println(reedSensorStatus ? "Circuit Broken" : "Circuit Closed");

  // If any ultrasonic sensor detects an object
  if (distance1 < 10) {
    triggerIntrusion(LIGHT1);  // Trigger LED and buzzer for side 1
    intrusionDetected = true;
  }
  if (distance2 < 8) {
    triggerIntrusion(LIGHT3);  // Trigger LED and buzzer for side 3
    intrusionDetected = true;
  }
  if (distance3 < 10) {
    triggerIntrusion(LIGHT4);  // Trigger LED and buzzer for side 4
    sendSMS("Object detected on side 4!");  // Send SMS if side 4 detects object
    intrusionDetected = true;
  }

  // Check PIR Sensors for Motion Detection
  if (digitalRead(PIR1) == HIGH) {
    triggerIntrusion(LIGHT1);  // Trigger LED and buzzer for side 1 for motion
    intrusionDetected = true;
  }
  if (digitalRead(PIR2) == HIGH) {
    triggerIntrusion(LIGHT2);  // Trigger LED and buzzer for side 2 for motion
    intrusionDetected = true;
  }
  if (digitalRead(PIR3) == HIGH) {
    triggerIntrusion(LIGHT3);  // Trigger LED and buzzer for side 3 for motion
    intrusionDetected = true;
  }
  if (digitalRead(PIR4) == HIGH) {
    triggerIntrusion(LIGHT4);  // Trigger LED and buzzer for side 4 for motion
    intrusionDetected = true;
  }

  // Check Magnetic Reed Sensor for Circuit Break
  if (reedSensorStatus) { // Circuit is broken
    triggerAllIntrusion();  // Trigger all LEDs and buzzer for gate security
    sendSMS("CIRCUIT BROKEN IN THE FARM BY UNKNOWN PERSON.");
    intrusionDetected = true;
  }

  // If Intrusion Detected
  if (intrusionDetected) {
    // No extra action needed as LEDs and buzzers are handled in the function
  } else {
    deactivateBuzzer();  // Turn off buzzer if no intrusion detected
  }

  delay(100);  // Delay for a second to reduce spamming of messages
}

// Function to check Ultrasonic Sensors for Object Detection
int checkUltrasonic(int trigPin, int echoPin) {
  long duration;
  int distance;

  // Trigger the ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echo response
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2; // Convert duration to distance in cm

  return distance;
}

// Function to blink the LED for 1.5 seconds (simulating rapid flash)
void blinkLED(int lightPin) {
  unsigned long startMillis = millis(); // Store the start time
  while (millis() - startMillis < 1500) { // Repeat for 1.5 seconds
    digitalWrite(lightPin, HIGH);  // Turn on the LED
    delay(50);                    // Wait for 0.05 seconds
    digitalWrite(lightPin, LOW);   // Turn off the LED
    delay(50);                    // Wait for 0.05 seconds
  }
}

// Function to trigger LED and buzzer simultaneously
void triggerIntrusion(int lightPin) {
  digitalWrite(lightPin, HIGH);  // Turn on the LED
  digitalWrite(BUZZER, HIGH);    // Turn on the buzzer
  blinkLED(lightPin);            // Blink LED rapidly for 1.5 seconds
  digitalWrite(BUZZER, LOW);     // Turn off the buzzer
}

// Function to trigger all LEDs and buzzer simultaneously (for gate security)
void triggerAllIntrusion() {
  unsigned long startMillis = millis(); // Store the start time
  while (millis() - startMillis < 2000) {  // Run for 2 seconds
    // Blink LEDs rapidly
    digitalWrite(LIGHT1, HIGH);
    digitalWrite(LIGHT2, HIGH);
    digitalWrite(LIGHT3, HIGH);
    digitalWrite(LIGHT4, HIGH);
    delay(100);  // Keep LEDs on for 0.1 second
    digitalWrite(LIGHT1, LOW);
    digitalWrite(LIGHT2, LOW);
    digitalWrite(LIGHT3, LOW);
    digitalWrite(LIGHT4, LOW);
    delay(100);  // Keep LEDs off for 0.1 second
  }
  digitalWrite(BUZZER, HIGH);  // Turn on the buzzer for 2 seconds
  delay(2000);                  // Keep buzzer on for 2 seconds
  digitalWrite(BUZZER, LOW);    // Turn off the buzzer
}

// Function to deactivate the buzzer
void deactivateBuzzer() {
  digitalWrite(BUZZER, LOW);   // Turn off the buzzer
}

// Function to send SMS using GSM Module
void sendSMS(String message) {
  gsm.print("AT+CMGF=1\r");          // Set SMS mode
  delay(100);

  gsm.print("AT+CMGS=\"");            // Send SMS to specified phone number
  gsm.print(ownerPhoneNumber);        // Add the owner's phone number
  gsm.println("\"");
  delay(100);

  gsm.print(message);                 // Send the actual message
  delay(100);

  gsm.write(26);                      // Send CTRL+Z to send SMS
  delay(1000);

  // Wait for any responses from GSM module and print them for debugging
  while (gsm.available()) {
    Serial.write(gsm.read());
  }
}